#include <glspec.hh>


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
}