#include <function.hh>

#include <sstream>
#include <iomanip>


namespace moo
{
    static const char LF = '\n';

    static table name_list;
    static table type_list;

    static table symbol_list;
    static table output_list;


    template <usize count>
    static void   fill_listing( listing & out, const char * ( & list )[ count ] )
    {
        for( auto element : list )
        {
            for( auto pair : type_list )   if( pair.second.find( element ) != string::npos )
            {
                goto save;
            }

            for( auto pair : output_list ) if( pair.second.find( element ) != string::npos )
            {
                goto save;
            }

            continue;

        save:
            out.emplace( element );
        }
    }

    static string make_context_type( const string & type, const string & version )
    {
        string out = type;
        usize size = type.length( );
        usize spot = type.find( "GL" );

        if( spot < size )
        {
            out.insert( spot,      "::" );
            out.insert( spot,   version );
            out.insert( spot, "context" );
        }

        return out;
    }
}

namespace moo
{
    using function = context::function;


    function::~function( ) = default;

    function:: function( const function & other ) = default;

    function:: function( function &&      other ) = default;

    function:: function( string && name )
    {
        auto last = symbol_list.size( );
        auto pair = symbol_list.emplace( last, static_cast<string &&>( name ) );

        tag = pair.first->first;

        append_output( "GLvoid" );
    }


    function & function::operator=( const function & other ) = default;

    function & function::operator=( function &&      other ) = default;


    string & function::name( )
    {
        return symbol_list.at( tag );
    }

    const string & function::name( ) const
    {
        return symbol_list.at( tag );
    }

    string function::context_name( ) const
    {
        auto starts = name( ).find( "gl" );

        if( starts == 0 ) return name( ).c_str( ) + 2;
        else              return name( );
    }


    string function::declaration( ) const
    {
        static usize tab;

        if( tab == 0 ) for( const auto & pair : output_list )
        {
            tab = std::max( tab, pair.second.length( ) );
        }


        string str;
        usize  len = output_list.at( out ).length( );
        usize  all = types.size( );

        str.append( output_list.at( out ) ).append( tab - len + 1, ' ' );
        str.append( context_name( ) ).append( 1, '(' );

        for( usize i = 0; i < all; ++i )
        {
            str.append( 1, ' ' ).append( type_list.at( types[ i ] ) );
            str.append( 1, ' ' ).append( name_list.at( names[ i ] ) );

            if( i != all - 1 ) str.append( 1, ',' );
        }

        str.append( " )" );


        return str;
    }

    string function::point( ) const
    {
        static usize tag_tab;
        static usize out_tab;

        if( tag_tab == 0 )
        {
            for( const auto & pair : symbol_list )
            {
                tag_tab = std::max( tag_tab, pair.second.length( ) );
            }

            tag_tab += 2;
        }

        if( out_tab == 0 ) for( const auto & pair : output_list )
        {
            out_tab = std::max( out_tab, pair.second.length( ) );
        }


        string str;
        usize  tag_len = symbol_list.at( tag ).length( );
        usize  out_len = output_list.at( out ).length( );
        usize  all     = types.size( );

        str.append( output_list.at( out ) ).append( out_tab - out_len + 1, ' ' );
        str.append( " ( * " );
        str.append( symbol_list.at( tag ) ).append( tag_tab - tag_len + 1, ' ' );
        str.append(    ")(" );

        for( usize i = 0; i < all; ++i )
        {
            str.append( 1, ' ' ).append( type_list.at( types[ i ] ) );

            if( i != all - 1 ) str.append( 1, ',' );
        }

        str.append( " )" );

        return str;
    }

    void function::implementation( std::ostream & stream, const string & version ) const
    {
        static usize tag_tab;
        static usize out_tab;

        if( tag_tab == 0 ) for( const auto & pair : symbol_list )
        {
            tag_tab = std::max( tag_tab, pair.second.length( ) );
        }

        if( out_tab == 0 )
        {
            for( const auto & pair : output_list )
            {
                out_tab = std::max( out_tab, pair.second.length( ) );
            }

            out_tab += version.length( );
            out_tab += 9;
        }


        string back = make_context_type( output_list.at( out ), version );

        stream << std::left << std::setw( 4 ) << ' ';
        stream << std::setw( out_tab );
        stream << back                  << " context";
        stream << version               << "::";
        stream << context_name( )       << "(";


        usize i;
        usize len = types.size( );

        for( usize i = 0; i < len; ++i )
        {
            stream << ' ' << type_list[ types[ i ] ];
            stream << ' ' << name_list[ names[ i ] ];

            if( i != len - 1 ) stream << ',';
        }

        stream << ' ' << ')'                   << LF;
        stream << std::setw( 4 ) << ' ' << '{' << LF;



        if( name( ) == "glGetError" )
        {
            stream << std::setw( 8 ) << ' ';
            stream << "return ( * call->glGetError )( );" << LF;
            stream << std::setw( 4 ) << ' ';
            stream << '}'                                 << LF;
        }
        else if( output_list.at( out ) == "GLvoid" )
        {
            stream << std::setw( 8 ) << ' ';
            stream << "IMPLEMENT_VOID( " << context_name( );

            for( i = 0; i < len; ++i )
            {
                stream << ',' << ' ' << name_list[ names[ i ] ];
            }

            stream << " )" << LF;
            stream << std::setw( 4 ) << ' ';
            stream << '}' << LF;
        }
        else
        {
            stream << std::setw( 8 ) << ' ';
            stream << "IMPLEMENT_TYPE( " << context_name( );

            for( i = 0; i < len; ++i )
            {
                stream << ',' << ' ' << name_list[ names[ i ] ];
            }

            stream << " )" << LF;
            stream << std::setw( 4 ) << ' ';
            stream << '}' << LF;
        }
    }


    void function::append_output( string && type )
    {
        for( auto pair : output_list ) if( pair.second == type )
        {
            out = pair.first;

            return;
        }

        auto last = output_list.size( );
        auto pair = output_list.emplace( last, static_cast<string &&>( type ) );

        out = pair.first->first;
    }

    void function::append_params( string && type, string && name )
    {
        bool   found;
        serial ttype, tname;

        for( found = false; auto pair : type_list ) if( pair.second == type )
        {
            found = true;
            ttype = pair.first;

            break;
        }

        if( not found )
        {
            auto last = type_list.size( );
            auto pair = type_list.emplace( last, static_cast<string &&>( type ) );

            ttype = pair.first->first;
        }


        for( found = false; auto pair : name_list ) if( pair.second == name )
        {
            found = true;
            tname = pair.first;

            break;
        }

        if( not found )
        {
            auto last = name_list.size( );
            auto pair = name_list.emplace( last, static_cast<string &&>( name ) );

            tname = pair.first->first;
        }


        types.emplace_back( ttype );
        names.emplace_back( tname );
    }


    void function::types_in_use( listing & integral, listing & floating, listing & versatile )
    {
        static const char * integral_list[]
        {
            "GLbyte",   "GLubyte",
            "GLshort",  "GLushort",
            "GLint",    "GLuint",
            "GLint64",  "GLuint64",
            "GLintptr", "GLsizeiptr"
        };

        static const char * floating_list[]
        {
            "GLdouble", "GLfloat", "GLhalf",
            "GLclampd", "GLclampf"
        };

        static const char * versatile_list[]
        {
            "GLsizei", "GLfixed",
            "GLvoid",  "GLboolean",
            "GLchar",  "GLbitfield",
            "GLenum",  "GLsync"
        };


        integral.clear( ), versatile.clear( ),
        floating.clear( );

        fill_listing(  integral,  integral_list );
        fill_listing(  floating,  floating_list );
        fill_listing( versatile, versatile_list );
    }
}