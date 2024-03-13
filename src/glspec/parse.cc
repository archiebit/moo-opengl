#include <parse.hh>
#include <constant.hh>
#include <function.hh>

#include <iostream>


namespace moo
{
    static void trim( std::string & string )
    {
        while( string.length( ) > 0 and string.back( ) == ' ' )
        {
            string.pop_back( );
        }

        while( string.length( ) > 0 and string.front( ) == ' ' )
        {
            string.erase( 0, 1 );
        }
    }

    static void nice( std::string & type )
    {
        auto i = std::string::size_type( 0 );
        auto n = std::string::npos;

        for( i = type.find( "void" ); i != n; i = type.find( "void", i + 6 ) )
        {
            type.insert( i, "GL" );
        }

        for( i = type.find( '*' ); i != n; i = type.find( '*', i ) )
        {
            auto prev = type[ i - 1 ];
            auto next = type[ i + 1 ];

            if( next != '*' and next != ' ' and next != 0 )
            {
                type.insert( i + 1, " " ); i += 1;
                continue;
            }

            if( prev != '*' and prev != ' ' )
            {
                type.insert( i - 1, " " ); i += 2;
                continue;
            }

            i += 1;
        }
    }

    static bool type_part( const rapidxml::xml_node<char> * node )
    {
        using rapidxml::node_data;
        using rapidxml::node_element;

        switch( node->type( ) )
        {
            case node_data:    return true;
            case node_element: return std::strcmp( node->name( ), "ptype" ) == 0;

            default:           return false;
        }

        return false;
    }

    static bool name_part( const rapidxml::xml_node<char> * node )
    {
        using rapidxml::node_element;

        if( node->type( ) != node_element )
        {
            return false;
        }

        return std::strcmp( node->name( ), "name" ) == 0;
    }
}
namespace moo
{
    parse_exception::~parse_exception( ) = default;

    parse_exception:: parse_exception( ) = default;


    parse::~parse( )
    { }

    parse:: parse( )
    { }


    void parse::init( )
    {
        std::memset( this, 0, sizeof( parse ) );
    }

    void parse::exit( )
    {
        delete   data;
        delete[] buff;
    }


    void parse::load( std::istream & source )
    {
        using io = std::istream;

        auto state = source.exceptions( );

        try
        {
            source.exceptions( io::failbit | io::badbit | io::eofbit );

            source.seekg( 0, io::end );

            std::size_t size = source.tellg( );

            buff = new char[ size + 1 ]( );

            source.seekg( 0, io::beg );

            source.read( buff, size );

            source.exceptions( state );
        }
        catch( std::exception & )
        {
            std::cerr << "Problem occured. Program can not open or read file.";
            std::cerr << std::endl;

            throw parse_exception( );
        }

        data = new document;

        data->parse<rapidxml::parse_default>( buff );


        regis = data->first_node( "registry" );

        if( not regis )
        {
            std::cerr << "File contents are invalid.";
            std::cerr << std::endl;

            throw parse_exception( );
        }

        root  = regis;
        enums = regis->first_node(    "enums" );
        comms = regis->first_node( "commands" );
        feats = regis->first_node(  "feature" );

        if( not enums or not comms or not feats )
        {
            std::cerr << "File contents are invalid.";
            std::cerr << std::endl;

            throw parse_exception( );
        }
    }

    void parse::save( int major, int minor, bool compatible )
    {
        int maj = 1;
        int min = 0;

        const node * feat = nullptr;
        const node * part = nullptr;
        const node * elem = nullptr;

        while( true )
        {
            if( not ( feat = version( maj, min ) ) )
            {
                maj += 1;
                min -= min;

                continue;
            }

            if( feat )
            {
                for( part = require( feat, compatible ); part; part = require( part, compatible, true ) )
                {
                    for( elem = part->first_node( "enum" ); elem; elem = elem->next_sibling( "enum" ) )
                    {
                        save_constant( find_constant( elem ) );
                    }

                    for( elem = part->first_node( "command" ); elem; elem = elem->next_sibling( "command" ) )
                    {
                        save_function( find_function( elem ) );
                    }
                }

                for( part = removes( feat, compatible ); part; part = removes( part, compatible, true ) )
                {
                    for( elem = part->first_node( "enum" ); elem; elem = elem->next_sibling( "enum" ) )
                    {
                        drop_constant( elem );
                    }

                    for( elem = part->first_node( "command" ); elem; elem = elem->next_sibling( "command" ) )
                    {
                        drop_function( elem );
                    }
                }
            }

            if( maj >= major and min >= minor ) break;

            min += 1;
        }
    }



    const parse::node * parse::version( int major, int minor )
    {
        char num[] = "0.0"; num[ 0 ] += major; num[ 2 ] += minor;
        char ver[] =  "gl";

        if( not root ) throw parse_exception( );


        auto step = root->first_node( "feature" );

        while( step )
        {
            auto api    = step->first_attribute(    "api" );
            auto number = step->first_attribute( "number" );

            if( not api or not number )
            {
                throw parse_exception( );
            }

            auto a = std::strcmp(    api->value( ), ver ) == 0;
            auto b = std::strcmp( number->value( ), num ) == 0;

            if( a and b ) return step;

            step = step->next_sibling( "feature" );
        }

        return nullptr;
    }

    const parse::node * parse::require( const node * in, bool compatible, bool next )
    {
        const char core[] = "core";
        const char comp[] = "compatibility";

        const node * step;

        if( next ) step = in->next_sibling( "require" );
        else       step = in->first_node( "require" );

        while( step )
        {
            auto prof = step->first_attribute( "profile" );


            if( not prof ) return step;

            if( std::strcmp( prof->value( ), core ) == 0 and not compatible )
            {
                return step;
            }

            if( std::strcmp( prof->value( ), comp ) == 0 and compatible )
            {
                return step;
            }

            step = step->next_sibling( "require" );
        }

        return nullptr;
    }

    const parse::node * parse::removes( const node * in, bool compatible, bool next )
    {
        const char core[] = "core";
        const char comp[] = "compatibility";

        const node * step;

        if( next ) step = in->next_sibling( "remove" );
        else       step = in->first_node( "remove" );

        while( step )
        {
            auto prof = step->first_attribute( "profile" );


            if( not prof ) return step;

            if( std::strcmp( prof->value( ), core ) == 0 and not compatible )
            {
                return step;
            }

            if( std::strcmp( prof->value( ), comp ) == 0 and compatible )
            {
                return step;
            }

            step = step->next_sibling( "remove" );
        }

        return nullptr;
    }


    const parse::node * parse::find_constant( const node * want )
    {
        auto name = want->first_attribute( "name" );

        if( not name )
        {
            return nullptr;
        }


        for( auto fold = enums;                      fold; fold = fold->next_sibling( "enums" ) )
        for( auto step = fold->first_node( "enum" ); step; step = step->next_sibling(  "enum" ) )
        {
            auto tag = step->first_attribute(  "name" );
            auto val = step->first_attribute( "value" );

            if( tag and val and std::strcmp( tag->value( ), name->value( ) ) == 0 )
            {
                return step;
            }
        }

        return nullptr;
    }

    const parse::node * parse::find_function( const node * want )
    {
        auto name = want->first_attribute( "name" );

        if( not name )
        {
            return nullptr;
        }


        for( auto fold = comms;                         fold; fold = fold->next_sibling( "commands" ) )
        for( auto step = fold->first_node( "command" ); step; step = step->next_sibling(  "command" ) )
        {
            const node * proto = step->first_node( "proto" );
            const node * tag   = nullptr;

            if( proto )
            {
                tag = proto->first_node( "name" );
            }

            if( tag and std::strcmp( tag->value( ), name->value( ) ) == 0 )
            {
                return step;
            }
        }

        return nullptr;
    }


    void parse::save_constant( const node * info )
    {
        auto name  = info->first_attribute(  "name" );
        auto value = info->first_attribute( "value" );


        std::string   tag = name->value( );
        std::uint64_t val = std::strtoull( value->value( ), nullptr, 0 );

        constants.emplace_back( static_cast<std::string &&>( tag ), val );
    }

    void parse::save_function( const node * info )
    {
        std::vector<std::string> types;
        std::vector<std::string> names;
        std::string tag;
        std::string out;

        const node * fold = info->first_node( "proto" );
        const node * step = nullptr;


        for( step = fold->first_node( ); step; step = step->next_sibling( ) )
        {
            std::string temp;

            if( type_part( step ) )
            {
                trim( temp = step->value( ) );

                if( out.length( ) > 0 and out.back( ) != ' ' )
                {
                    out.append( 1, ' ' );
                }

                out += temp;
            }
            else if( name_part( step ) )
            {
                trim( tag = step->value( ) );
            }
            else throw parse_exception( );
        }

        nice( out );

        for( fold = info->first_node( "param" ); fold; fold = fold->next_sibling( "param" ) )
        {
            std::string type;
            std::string name;

            for( step = fold->first_node( ); step; step = step->next_sibling( ) )
            {
                std::string temp;

                if( type_part( step ) )
                {
                    trim( temp = step->value( ) );

                    if( type.length( ) > 0 and type.back( ) != ' ' )
                    {
                        type.append( 1, ' ' );
                    }

                    type += temp;
                }
                else if( name_part( step ) )
                {
                    trim( name = step->value( ) );
                }
                else throw parse_exception( );
            }

            nice( type );

            types.emplace_back( static_cast<std::string &&>( type ) );
            names.emplace_back( static_cast<std::string &&>( name ) );
        }


        functions.emplace_back( std::move( tag ), std::move( out ), std::move( types ), std::move( names ) );
    }


    void parse::drop_constant( const node * info )
    {
        auto label = info->first_attribute( "name" )->value( );
        auto index = decltype( constants )::size_type( );

        for( index = 0; index < constants.size( ); ++index )
        {
            if( constants[ index ].name( ) == label )
            {
                constants.erase( constants.cbegin( ) + index );
                break;
            }
        }
    }

    void parse::drop_function( const node * info )
    {
        auto label = info->first_attribute( "name" )->value( );
        auto index = decltype( functions )::size_type( );

        for( index = 0; index < functions.size( ); ++index )
        {
            if( functions[ index ].name( ) == label )
            {
                functions.erase( functions.cbegin( ) + index );
                break;
            }
        }
    }
}