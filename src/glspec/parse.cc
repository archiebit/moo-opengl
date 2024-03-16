#include <parse.hh>
#include <constant.hh>
#include <function.hh>

#include <iostream>
#include <fstream>


namespace moo
{
    using document  = parse::document;
    using node      = parse::node;
    using attribute = parse::attribute;


    static string & make_nice_type( string & s )
    {
        for( auto i = s.find( "void" ); i < s.length( ); )
        {
            s.insert( i, "GL" );

            i = s.find( "void", i + 6 );
        }

        for( auto i = s.find(    '*' ); i < s.length( ); )
        {
            auto prev = i == 0 ? 0 : s[ i - 1 ];
            auto next = s[ i + 1 ];

            if( next != '*' and next != ' ' and next != 0 )
            {
                s.insert( i + 1, 1, ' ' );

                i = s.find( '*', i + 1 );

                continue;
            }

            if( prev != '*' and prev != ' ' and prev != 0 )
            {
                s.insert( i, 1, ' ' );

                i = s.find( '*', i + 2 );

                continue;
            }

            i += 1;
        }

        return s;
    }


    static bool type_part( const node * in )
    {
        using rapidxml::node_data;
        using rapidxml::node_element;


        switch( in->type( ) )
        {
            case node_data:    return true;
            case node_element: return std::strcmp( in->name( ), "ptype" ) == 0;

            default:           return false;
        }
    }

    static bool name_part( const node * in )
    {
        using rapidxml::node_element;


        if( in->type( ) != node_element )
        {
            return false;
        }

        return std::strcmp( in->name( ), "name" ) == 0;
    }


    static string & prepare_type( const char s[], string & output )
    {
        static string temp;


        temp = s;

        while( temp.length( ) > 0 and temp.back( ) == ' ' )
        {
            temp.pop_back( );
        }

        while( temp.length( ) > 0 and temp.front( ) == ' ' )
        {
            temp.erase( 0, 1 );
        }

        if( temp.length( ) == 0 )
        {
            throw parse_exception( );
        }


        if( output.length( ) > 0 ) output.append( 1, ' ' ).append( temp );
        else                       output.append( temp );


        return output;
    }

    static string & prepare_name( const char s[], string & symbol )
    {
        static string temp;


        temp = s;

        while( temp.length( ) > 0 and temp.back( ) == ' ' )
        {
            temp.pop_back( );
        }

        while( temp.length( ) > 0 and temp.front( ) == ' ' )
        {
            temp.erase( 0, 1 );
        }


        if( temp.length( ) == 0 or symbol.length( ) != 0 )
        {
            throw parse_exception( );
        }


        return symbol.append( temp );
    }


    static bool find( const node *& child, const node * parent, const char name[] = nullptr )
    {
        if( not parent )
        {
            throw parse_exception( );
        }

        return child = parent->first_node( name );
    }

    static bool next( const node *& child, const char name[] = nullptr )
    {
        if( not child )
        {
            throw parse_exception( );
        }

        return child = child->next_sibling( name );
    }
}


namespace moo
{
    parse_exception::~parse_exception( ) = default;

    parse_exception:: parse_exception( ) = default;
}

namespace moo
{
    parse::~parse( ) = default;

    parse:: parse( ) = default;


    void parse::init( )
    {
        data = nullptr;
        buff = nullptr;
        root = nullptr;
    }

    void parse::exit( )
    {
        delete   data;
        delete[] buff;
    }


    void parse::load( const string & filename )
    {
        using stream = std::ifstream;

        stream source;
        usize  total;

        try
        {
            source.exceptions( ~stream::goodbit );
            source.open( filename );

            source.seekg( 0, stream::end ), total = source.tellg( );
            source.seekg( 0, stream::beg );

            buff = new char[ total + 1 ]( );

            source.read( buff, total );
        }
        catch( std::exception & )
        {
            std::cerr << "Problem occured. Program can not open or read file.";
            std::cerr << std::endl;

            throw parse_exception( );
        }

        data = new document;

        data->parse<rapidxml::parse_default>( buff );

        if( not find( root, data, "registry" ) )
        {
            std::cerr << "File contents are invalid.";
            std::cerr << std::endl;

            throw parse_exception( );
        }
    }

    void parse::save( int major, int minor, bool compatible )
    {
        const node * feat;
        const node * fold;
        const node * step;

        for( int maj = 1, min = 0; maj <= major and min <= minor; )
        {
            if( not version( feat, maj, min ) )
            {
                maj += 1;
                min -= min;

                continue;
            }


            static const char core[] = "core";
            static const char comp[] = "compatibility";

            static const char cons[] = "enum";
            static const char func[] = "command";


            for( find( fold, feat, "require" ); fold; next( fold, "require" ) )
            {
                if( auto profile = fold->first_attribute( "profile" ) )
                {
                    auto is_core = std::strcmp( profile->value( ), core ) == 0;
                    auto is_comp = std::strcmp( profile->value( ), comp ) == 0;

                    if( is_core and     compatible ) continue;
                    if( is_comp and not compatible ) continue;
                }


                for( find( step, fold, cons ); step; next( step, cons ) )
                {
                    parse_constant( find_constant( step ) );
                }

                for( find( step, fold, func ); step; next( step, func ) )
                {
                    parse_function( find_function( step ) );
                }
            }

            for( find( fold, feat,  "remove" ); fold; next( fold,  "remove" ) )
            {
                if( auto profile = fold->first_attribute( "profile" ) )
                {
                    auto is_core = std::strcmp( profile->value( ), core ) == 0;
                    auto is_comp = std::strcmp( profile->value( ), comp ) == 0;

                    if( is_core and     compatible ) continue;
                    if( is_comp and not compatible ) continue;
                }


                for( find( step, fold, cons ); step; next( step, cons ) )
                {
                    erase_function( step );
                }

                for( find( step, fold, func ); step; next( step, func ) )
                {
                    erase_function( step );
                }
            }

            min += 1;
        }
    }


    bool parse::version( const node *& feat, int major, int minor )
    {
        char num[] = "0.0"; num[ 0 ] += major; num[ 2 ] += minor;
        char ver[] =  "gl";

        for( find( feat, root, "feature" ); feat; next( feat, "feature" ) )
        {
            auto family = feat->first_attribute(    "api" );
            auto number = feat->first_attribute( "number" );

            if( not family or not number )
            {
                throw parse_exception( );
            }

            auto a = std::strcmp( family->value( ), ver ) == 0;
            auto b = std::strcmp( number->value( ), num ) == 0;

            if( a and b ) return true;
        }

        return false;
    }


    const node * parse::find_constant( const node * in )
    {
        if( not in or not in->first_attribute( "name" ) )
        {
            throw parse_exception( );
        }

        const char * want = in->first_attribute( "name" )->value( );
        const node * fold;
        const node * step;

        for( find( fold, root, "enums" ); fold; next( fold, "enums" ) )
        for( find( step, fold,  "enum" ); step; next( step,  "enum" ) )
        {
            auto name = step->first_attribute(  "name" );
            auto uval = step->first_attribute( "value" );

            if( not name or not uval ) throw parse_exception( );


            if( std::strcmp( name->value( ), want ) == 0 )
            {
                return step;
            }
        }

        return nullptr;
    }

    const node * parse::find_function( const node * in )
    {
        if( not in or not in->first_attribute( "name" ) )
        {
            throw parse_exception( );
        }


        const char * want = in->first_attribute( "name" )->value( );
        const node * fold;
        const node * step;

        for( find( fold, root, "commands" ); fold; next( fold, "commands" ) )
        for( find( step, fold,  "command" ); step; next( step,  "command" ) )
        {
            const node * prot = step->first_node( "proto" );
            const node * name = nullptr;

            if( not prot or not ( name = prot->first_node( "name" ) ) )
            {
                throw parse_exception( );
            }


            if( std::strcmp( name->value( ), want ) == 0 )
            {
                return step;
            }
        }

        return nullptr;
    }


    void parse::parse_constant( const node * in )
    {
        if( not in ) throw parse_exception( );


        auto label = in->first_attribute(  "name" );
        auto value = in->first_attribute( "value" );

        string name = label->value( );
        uint   uval = std::strtoull( value->value( ), nullptr, 0 );

        constants.emplace_back( static_cast<string &&>( name ), uval );
    }

    void parse::parse_function( const node * in )
    {
        if( not in ) throw parse_exception( );


        strings  types,  names;
        string  symbol, output;

        const node * fold = in->first_node( "proto" );
        const node * step;
        const node * part;


        for( find( step, fold ); step; next( step ) )
        {
            if( type_part( step ) )
            {
                prepare_type( step->value( ), output );
            }
            else if( name_part( step ) )
            {
                prepare_name( step->value( ), symbol );
            }
            else throw parse_exception( );
        }


        for( find( step,   in, "param" ); step; next( step, "param" ) )
        {
            auto & type = types.emplace_back( );
            auto & name = names.emplace_back( );

            for( find( part, step ); part; next( part ) )
            {
                if( type_part( part ) )
                {
                    prepare_type( part->value( ), type );
                }
                else if( name_part( part ) )
                {
                    prepare_name( part->value( ), name );
                }
                else throw parse_exception( );
            }
        }


        auto & curr = functions.emplace_back( std::move( symbol ) );

        curr.append_output( std::move( make_nice_type( output ) ) );

        for( usize i = 0; i < types.size( ); ++i )
        {
            auto & type = make_nice_type( types[ i ] );
            auto & name = names[ i ];

            curr.append_params( std::move( type ), std::move( name ) );
        }
    }


    void parse::erase_constant( const node * in )
    {
        if( not in ) throw parse_exception( );


        string name = in->first_attribute( "name" )->value( );
        usize   all = constants.size( );

        for( usize i = 0; i < all; ++i ) if( constants[ i ].name( ) == name )
        {
            constants.erase( constants.cbegin( ) + i );

            break;
        }
    }

    void parse::erase_function( const node * in )
    {
        if( not in ) throw parse_exception( );


        string name = in->first_attribute( "name" )->value( );
        usize   all = functions.size( );

        for( usize i = 0; i < all; ++i ) if( functions[ i ].name( ) == name )
        {
            functions.erase( functions.cbegin( ) + i );

            break;
        }
    }
}