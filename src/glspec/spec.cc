#include <glspec.hh>


namespace moo
{
    bool context::check_command_line( int argc, char * argv[] )
    {
        if( argc < 3 or argc > 4 or not argv )
        {
            return false;
        }

        auto version  = false;
        auto profile  = false;
        auto filename = false;

        for( int i = 1; i < argc; ++i )
        {
            auto string = argv[ i ];

            if( std::strncmp( string, "/V", 2 ) == 0 )
            {
                if( version ) return false;

                unsigned int maj;
                unsigned int min;

                if( std::sscanf( string, "/V:%1u.%1u", & maj, & min ) != 2 )
                {
                    return false;
                }

                version = true;

                continue;
            }

            if( std::strncmp( string, "/P", 2 ) == 0 )
            {
                if( profile ) return false;

                if( std::strcmp( string, "/P:core"       ) == 0 ) profile = true;
                if( std::strcmp( string, "/P:compatible" ) == 0 ) profile = true;

                if( not profile )
                {
                    return false;
                }

                continue;
            }

            if( std::strncmp( string, "/I", 2 ) == 0 )
            {
                if( filename ) return false;

                char test;

                if( std::sscanf( string, "/I:%c", & test ) != 1 )
                {
                    return false;
                }

                filename = true;

                continue;
            }
        }

        return version and profile and filename;
    }
}