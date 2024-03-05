#include <glspec.hh>
#include <rapidxml.hh>

#include <iostream>
#include <fstream>


namespace xml = rapidxml;

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
}
namespace moo
{
    using document  = xml::xml_document<char>;
    using attribute = xml::xml_attribute<char>;
    using node      = xml::xml_node<char>;




    static const node * find_version( const node * root, ordinal maj, ordinal min )
    {
        if( not root ) return nullptr;

        const node * regs = root->first_node( "registry" );
        const node * step = nullptr;

        if( not regs )
        {
            return nullptr;
        }
        else step = regs->first_node( "feature" );


        char name[]    = "gl";
        char version[] = "_._";


        for( step; step; step = step->next_sibling( "featue" ) )
        {
            auto api = step->first_attribute(    "api" );
            auto num = step->first_attribute( "number" );

            if( api and num )
            {
                auto a = std::strcmp( api->value( ),    name ) == 0;
                auto b = std::strcmp( num->value( ), version ) == 0;

                if( a and b ) break;
            }
        }

        return step;
    }


    static const node * find_requirement( const node * feature, profile type )
    {
        if( not feature ) return nullptr;


        const node * step = feature->first_node( "require" );

        char core[] = "core";
        char comp[] = "compatibility";


        for( step; step; step = step->next_sibling( "require" ) )
        {
            auto prof = step->first_attribute( "profile" );
            auto test = NONE;

            if( prof )
            {
                if( std::strcmp( prof->value( ), core ) == 0 ) test = CORE;
                if( std::strcmp( prof->value( ), comp ) == 0 ) test = COMPATIBLE;
            }

            if( test == type or test == NONE )
            {
                break;
            }
        }

        return step;
    }

    static const node * next_requirement( const node * sibling, profile type )
    {
        if( not sibling ) return nullptr;


        const node * step = sibling;

        char core[] = "core";
        char comp[] = "compatibility";


        for( step; step; step = step->next_sibling( "require" ) )
        {
            auto prof = step->first_attribute( "profile" );
            auto test = NONE;

            if( prof )
            {
                if( std::strcmp( prof->value( ), core ) == 0 ) test = CORE;
                if( std::strcmp( prof->value( ), comp ) == 0 ) test = COMPATIBLE;
            }

            if( test == type or test == NONE )
            {
                break;
            }
        }

        return step;
    }

    static const node * find_removement( const node * feature, profile type )
    {
        if( not feature ) return nullptr;


        const node * step = feature->first_node( "remove" );

        char core[] = "core";
        char comp[] = "compatibility";


        for( step; step; step = step->next_sibling( "remove" ) )
        {
            auto prof = step->first_attribute( "profile" );
            auto test = NONE;

            if( prof )
            {
                if( std::strcmp( prof->value( ), core ) == 0 ) test = CORE;
                if( std::strcmp( prof->value( ), comp ) == 0 ) test = COMPATIBLE;
            }

            if( test == type or test == NONE )
            {
                break;
            }
        }

        return step;
    }

    static const node * next_removement( const node * sibling, profile type )
    {
        if( not sibling ) return nullptr;


        const node * step = sibling->first_node( "remove" );

        char core[] = "core";
        char comp[] = "compatibility";


        for( step; step; step = step->next_sibling( "remove" ) )
        {
            auto prof = step->first_attribute( "profile" );
            auto test = NONE;

            if( prof )
            {
                if( std::strcmp( prof->value( ), core ) == 0 ) test = CORE;
                if( std::strcmp( prof->value( ), comp ) == 0 ) test = COMPATIBLE;
            }

            if( test == type or test == NONE )
            {
                break;
            }
        }

        return step;
    }


    static const node * find_constant( const node * what, const node * root )
    {
        if( not what or not root ) return nullptr;

        const node * high = root->first_node( "enums" );
        const node * step = nullptr;

        if( not high )
        {
            return nullptr;
        }
        else step = high->first_node( "enum" );


        const attribute * tag = what->first_attribute( "name" );
        const attribute * val = nullptr;
        const attribute * tmp = nullptr;

        if( not tag ) return nullptr;


        for( high; high; high = high->next_sibling( "enums" ) )
        for( step; step; step = step->next_sibling( "enum"  ) )
        {
            tmp = step->first_attribute(  "name" );
            val = step->first_attribute( "value" );

            if( tmp and val and std::strcmp( tmp->value( ), tag->value( ) ) == 0 )
            {
                break;
            }
        }

        return step;
    }

    static const node * find_function( const node * what, const node * root )
    {
        if( not what or not root ) return nullptr;

        const node * high = root->first_node( "commands" );
        const node * step = nullptr;

        if( not high )
        {
            return nullptr;
        }
        else step = high->first_node( "command" );


        const attribute * tag = what->first_attribute( "name" );
        const node *      pro = nullptr;
        const node *      tmp = nullptr;
        std::string       spt;

        if( not tag ) return nullptr;


        for( high; high; high = high->next_sibling( "commands" ) )
        for( step; step; step = step->next_sibling( "command"  ) )
        {
            bool done
            {
                ( pro = step->first_node( "proto" ) )
                and
                ( tmp =  pro->first_node(  "name" ) )
            };

            if( done )
            {
                trim( spt = tmp->value( ) );

                if( std::strcmp( spt.c_str( ), tag->value( ) ) == 0 )
                {
                    break;
                }
            }
        }

        return step;
    }
}

namespace moo
{
    bool context::check_command_line( int argc, char * argv[], ordinal & major, ordinal & minor, profile & type, std::string & inputs )
    {
        if( argc < 3 or argc > 4 or not argv )
        {
            return false;
        }

        bool version  = false;
        bool profile  = false;
        bool filename = false;

        major = 0;
        minor = 0;
        type  = NONE;

        for( int i = 1; i < argc; ++i )
        {
            auto string = argv[ i ];

            if( std::strncmp( string, "/V", 2 ) == 0 )
            {
                if( version ) return false;

                unsigned int maj = 0;
                unsigned int min = 0;

                if( std::sscanf( string, "/V:%1u.%1u", & maj, & min ) != 2 )
                {
                    return false;
                }

                version = true;
                major   = maj;
                minor   = min;

                continue;
            }

            if( std::strncmp( string, "/P", 2 ) == 0 )
            {
                if( profile ) return false;

                if( std::strcmp( string, "/P:core"       ) == 0 )
                {
                    profile = true; type = CORE;
                }

                if( std::strcmp( string, "/P:compatible" ) == 0 )
                {
                    profile = true; type = COMPATIBLE;
                }

                continue;
            }

            if( std::strncmp( string, "/I", 2 ) == 0 )
            {
                if( filename ) return false;

                char c;

                if( std::sscanf( string, "/I:%c", & c ) != 1 )
                {
                    return false;
                }

                filename = true;
                inputs   = string + 3;

                continue;
            }
        }

        if( major >= 3 and minor >= 2 )
        {
            return version and profile and filename;
        }
        else return version and filename;
    }


    context context::parse( const std::string & filename, ordinal major, ordinal minor, profile type )
    {
        try
        {
            std::ifstream file;
            std::string   spot;
            document      root;


            file.exceptions( std::ifstream::goodbit );
            file.open( filename );

            file.seekg( 0, std::ifstream::end ), spot.append( file.tellg( ), ' ' );
            file.seekg( 0, std::ifstream::beg );

            file.read( spot.data( ), spot.size( ) );
            file.close( );

            root.parse<xml::parse_default>( spot.data( ) );
        }
        catch( std::exception & )
        {
            std::cerr << "Error occured when opening or reading file.";
            std::cerr << std::endl;

            std::exit( EXIT_FAILURE );
        }
    }
}