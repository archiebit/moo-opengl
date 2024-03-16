#include <function.hh>

#include <sstream>
#include <iomanip>


namespace moo
{
    static table name_list;
    static table type_list;

    static table symbol_list;
    static table output_list;
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
        str.append( symbol_list.at( tag ) ).append( 1, '(' );

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

        if( tag_tab == 0 ) for( const auto & pair : symbol_list )
        {
            tag_tab = std::max( tag_tab, pair.second.length( ) );
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
}