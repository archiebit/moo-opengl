#include <context.hh>
#include <function.hh>
#include <constant.hh>
#include <parse.hh>

#include <iomanip>
#include <sstream>


namespace moo
{
    std::string context::version( int major, int minor, bool compatible )
    {
        std::string result = "context<, , >";

        if( compatible ) result.insert( 12, "COMPATIBLE" );
        else             result.insert( 12,       "CORE" );

        result.insert( 10, 1, '0' + minor );
        result.insert(  8, 1, '0' + major );

        return result;
    }

    void context::declare_types( std::ostream & stream, std::size_t tab )
    {
        static const char * const integral[]
        {
            "// Integral types."
            "using GLbyte   = signed char;",
            "using GLshort  = signed short int;",
            "using GLint    = signed int;",
            "using GLint64  = signed long long int;",
            "",
            "using GLubyte  = unsigned char;",
            "using GLushort = unsigned short int;",
            "using GLuint   = unsigned int;",
            "using GLuint64 = unsigned long long int;",
            ""
            "using GLintptr = signed long long int;",
            "using GLsizeiptr = unsigned long long int;"
        };

        static const char * const floating[]
        {
            "// Floating types."
            "using GLfloat  = float;",
            "using GLdouble = double;",
            "using GLclampf = float;",
            "using GLclampd = double;",
            ""
            "enum GLhalf    : signed short int;"
        };

        static const char * const misclens[]
        {
            "// Misclenaus types."
            "using GLboolean  = bool;",
            "using GLchar     = char;",
            "using GLvoid     = void;",
            "using GLsizei    = unsigned int;",
            "using GLbitfield = unsigned int;",
            "using GLintptr   = signed long long int;",
            "using GLsizeiptr = unsigned long long int;",
            "using GLenum     = GLenum32",
            "",
            "enum GLsync      : unsigned long long int;",
            "enum GLfixed     : signed int;"
        };


        for( auto string : integral )
        {
            stream << std::setw( tab ) << string;
            stream << '\n';
        }

        for( auto string : floating )
        {
            stream << std::setw( tab ) << string;
            stream << '\n';
        }

        for( auto string : misclens )
        {
            stream << std::setw( tab ) << string;
            stream << '\n';
        }
    }

    void context::declare_begin( std::ostream & stream )
    {
        stream << std::left;

        static const char * const code[]
        {
            "namespace moo",
            "{",
            "    enum profile",
            "    {",
            "        CORE,",
            "        COMPATIBLE",
            "    };",
            "",
            "    template <int major, int minor, profile option>",
            "    class context;"
            "}"
        };

        stream << std::left;

        for( auto string : code )
        {
            stream << string << '\n';
        }
    }
}

namespace moo
{
    context::~context( ) = default;

    context:: context( ) = default;

    context:: context( context && other ) = default;

    context:: context( std::istream & source, int major, int minor, bool compatible )
    {
        auto & worker = static_cast<parse &>( * this );

        worker.init( );
        worker.load( source );
        worker.save( major, minor, compatible );
        worker.exit( );
    }


    void context::save_header( std::ostream & target )
    {
        for( auto & com : functions )
        {
            target << com.declaration( ) << '\n';
        }

        for( auto & com : functions )
        {
            target << com.point( )       << '\n';
        }
    }


    void context::save_constants( std::ostream & target )
    {
        std::vector<constant *> small;
        std::vector<constant *> large;
        std::uint32_t         maximum = std::numeric_limits<std::uint32_t>::max( );

        std::size_t small_tab = 0;
        std::size_t large_tab = 0;

        for( auto & enm : constants )
        {
            if( enm.value( ) <= maximum )
            {
                small.emplace_back( & enm );

                small_tab = std::max( small_tab, enm.name( ).length( ) );
            }
            else
            {
                large.emplace_back( & enm );

                large_tab = std::max( large_tab, enm.name( ).length( ) );
            }
        }




        target << "enum GLenum32 : unsigned int"    << '\n';
        target << "{"                               << '\n';

        for( auto enm : small )
        {
            target << std::left << std::setw( 4 ) << ' ';
            target << std::left << std::setw( small_tab );

            target << enm->name( );
            target << " = 0x";

            std::stringstream ss;
            std::string       st;

            ss << std::uppercase << std::hex << std::setfill( '0' );
            ss << std::right     << std::setw( 8 );
            ss << enm->value( );
            st = ss.str( );

            for( int i = 0; i < 8; ++i )
            {
                if( i % 4 == 0 and i != 0 ) target << '\'';

                target << st[ i ];
            }

            target << ',' << '\n';
        }

        target << "};"                              << '\n';




        target << "enum GLenum64 : unsigned long long int"  << '\n';
        target << "{"                                       << '\n';

        for( auto enm : large )
        {
            target << std::left << std::setw( 4 ) << ' ';
            target << std::left << std::setw( large_tab );

            target << enm->name( );
            target << " = 0x";

            std::stringstream ss;
            std::string       st;

            ss << std::uppercase << std::hex << std::setfill( '0' );
            ss << std::right     << std::setw( 16 );
            ss << enm->value( );
            st = ss.str( );

            for( int i = 0; i < 16; ++i )
            {
                if( i % 4 == 0 and i != 0 ) target << '\'';

                target << st[ i - 0 ];
            }

            target << ',' << '\n';
        }

        target << "};"                              << '\n';
    }
}