#include <function.hh>

#include <sstream>
#include <iomanip>


namespace moo
{
    std::vector<std::string> context::function::tags_all;
    std::vector<std::string> context::function::outs_all;
    std::vector<std::string> context::function::type_all;
    std::vector<std::string> context::function::name_all;
}

namespace moo
{
    context::function::~function( )
    { }

    context::function:: function( std::string && name, std::string && outs, std::vector<std::string> && type, std::vector<std::string> && args )
    {
        for( auto && element : type )
        {
            save_type( static_cast<std::string &&>( element ) );
        }

        for( auto && element : args )
        {
            save_name( static_cast<std::string &&>( element ) );
        }

        save_outs( static_cast<std::string &&>( outs ) );
        save_tags( static_cast<std::string &&>( name ) );
    }


    std::string & context::function::name( )
    {
        return tags_all[ tag ];
    }

    std::string context::function::declaration( )
    {
        static std::size_t tab;

        if( tab == 0 ) for( auto & outs : outs_all )
        {
            tab = std::max( tab, outs.length( ) );
        }


        std::string   s;
        std::size_t len = outs_all[ out ].length( );

        s.append( outs_all[ out ] );
        s.append( tab - len + 1, ' ' );

        s.append( tags_all[ tag ] );
        s.append( 1, '(' );


        for( std::size_t i = 0; i < types.size( ); ++i )
        {
            s.append( 1, ' ' ).append( type_all[ types[ i ] ] );
            s.append( 1, ' ' ).append( name_all[ names[ i ] ] );


            if( i != types.size( ) - 1 ) s.append( 1, ',' );
        }

        s.append( " )" );

        return s;
    }

    std::string context::function::point( )
    {
        static std::size_t tag_tab;
        static std::size_t out_tab;

        if( tag_tab == 0 ) for( const auto & tag : tags_all )
        {
            tag_tab = std::max( tag_tab, tag.length( ) );
        }

        if( out_tab == 0 ) for( const auto & out : outs_all )
        {
            out_tab = std::max( out_tab, out.length( ) );
        }


        std::string s;
        std::size_t len_tag = tags_all[ tag ].length( );
        std::size_t len_out = outs_all[ out ].length( );


        s.append( outs_all[ out ] ).append( out_tab - len_out + 1, ' ' );
        s.append( "( * " );
        s.append( tags_all[ tag ] ).append( tag_tab - len_tag + 1, ' ' );
        s.append(   ")(" );

        for( std::size_t i = 0; i < types.size( ); ++i )
        {
            s.append( 1, ' ' ).append( type_all[ types[ i ] ] );

            if( i != types.size( ) - 1 ) s.append( 1, ',' );
        }

        s.append( " )" );

        return s;
    }



    void context::function::save_outs( std::string && outs )
    {
        auto found = false;
        auto index = decltype( outs_all )::size_type( 0 );

        for( index = 0; index < outs_all.size( ); ++index )
        {
            if( outs_all[ index ] == outs )
            {
                found = true;
                break;
            }
        }

        if( not found )
        {
            outs_all.emplace_back( static_cast<std::string &&>( outs ) );
        }

        out = index;
    }

    void context::function::save_type( std::string && type )
    {
        auto found = false;
        auto index = decltype( type_all )::size_type( 0 );

        for( index = 0; index < type_all.size( ); ++index )
        {
            if( type_all[ index ] == type )
            {
                found = true;
                break;
            }
        }

        if( not found )
        {
            type_all.emplace_back( static_cast<std::string &&>( type ) );
        }

        types.emplace_back( index );
    }

    void context::function::save_name( std::string && name )
    {
        auto found = false;
        auto index = decltype( name_all )::size_type( 0 );

        for( index = 0; index < name_all.size( ); ++index )
        {
            if( name_all[ index ] == name )
            {
                found = true;
                break;
            }
        }

        if( not found )
        {
            name_all.emplace_back( static_cast<std::string &&>( name ) );
        }

        names.emplace_back( index );
    }

    void context::function::save_tags( std::string && name )
    {
        auto found = false;
        auto index = decltype( tags_all )::size_type( 0 );

        for( index = 0; index < tags_all.size( ); ++index )
        {
            if( tags_all[ index ] == name )
            {
                found = true;
                break;
            }
        }

        if( not found )
        {
            tags_all.emplace_back( static_cast<std::string &&>( name ) );
        }

        tag = index;
    }
}