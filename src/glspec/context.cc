#include <context.hh>
#include <function.hh>
#include <constant.hh>
#include <parse.hh>

#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>


namespace moo
{
    string context::version( )
    {
        std::string result = "context<, , >";

        if( compatible ) result.insert( 12, "COMPATIBLE" );
        else             result.insert( 12,       "CORE" );

        result.insert( 10, 1, '0' + minor );
        result.insert(  8, 1, '0' + major );

        return result;
    }

    void context::append_datatypes( std::ostream & stream )
    {
        static const char * const integral[]
        {
            "// Integral types.",
            "using GLbyte     = signed char;",
            "using GLshort    = signed short int;",
            "using GLint      = signed int;",
            "using GLint64    = signed long long int;",
            "",
            "using GLubyte    = unsigned char;",
            "using GLushort   = unsigned short int;",
            "using GLuint     = unsigned int;",
            "using GLuint64   = unsigned long long int;",
            "",
            "using GLintptr   = signed long long int;",
            "using GLsizeiptr = unsigned long long int;",
            ""
        };

        static const char * const floating[]
        {
            "// Floating types.",
            "using GLfloat  = float;",
            "using GLdouble = double;",
            "using GLclampf = float;",
            "using GLclampd = double;",
            "",
            "enum GLhalf    : signed short int { };",
            ""
        };

        static const char * const misclens[]
        {
            "// Misclenaus types.",
            "using GLboolean  = bool;",
            "using GLchar     = char;",
            "using GLvoid     = void;",
            "using GLsizei    = unsigned int;",
            "using GLbitfield = unsigned int;",
            "using GLintptr   = signed long long int;",
            "using GLsizeiptr = unsigned long long int;",
            "using GLenum     = GLenum32;",
            "",
            "enum GLsync      : unsigned long long int { };",
            "enum GLfixed     : signed int             { };",
            ""
        };


        for( auto string : integral )
        {
            stream << std::setw( 8 ) << ' ' << string << '\n';
        }

        for( auto string : floating )
        {
            stream << std::setw( 8 ) << ' ' << string << '\n';
        }

        for( auto string : misclens )
        {
            stream << std::setw( 8 ) << ' ' << string << '\n';
        }
    }

    void context::append_file_head( std::ostream & stream )
    {
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
            "    class context;",
            "}",
            ""
        };

        for( auto string : code )
        {
            stream << string << '\n';
        }
    }

    void context::class_head( std::ostream & stream )
    {
        static const char * const code[]
        {
            "namespace moo",
            "{",
            "    template <>",
            "    class ",
            "    {",
            "    public:",
            "       ~context( );",
            "        context( );",
            "        context( const context & ) = delete;",
            "        context( context &&      ) = delete;",
            ""
        };

        for( std::size_t i = 0; auto string : code )
        {
            stream << string;

            if( i == 3 ) stream << version( ) << '\n';
            else         stream << '\n';

            i += 1;
        }
    }

    void context::class_foot( std::ostream & stream )
    {
        static const char * const code[]
        {
            "",
            "    private:",
            "        void * implementation;",
            "    };",
            "}"
        };

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

    context:: context( const std::string & filename, int major, int minor, bool compatible )
    :   major( major ),
        minor( minor ),
        compatible( compatible )
    {
        auto & worker = static_cast<parse &>( * this );

        worker.init( );
        worker.load( filename );
        worker.save( major, minor, compatible );
        worker.exit( );
    }


    void context::save( )
    {
        std::ofstream impl;
        std::ofstream head;

        head.exceptions( ~std::ofstream::goodbit );
        impl.exceptions( ~std::ofstream::goodbit );

        head.open( "gl.hh", std::ofstream::trunc );
        impl.open( "gl.cc", std::ofstream::trunc );


        append_file_head( head );

        class_head( head );
        append_constants( head );
        append_datatypes( head );
        append_functions( head );
        class_foot( head );


        head.close( );
        impl.close( );
    }


    void context::append_constants( std::ostream & stream )
    {
        std::vector<constant *> small;
        std::vector<constant *> large;
        std::size_t         tab_small = 0;
        std::size_t         tab_large = 0;
        std::uint32_t      limitation = std::numeric_limits<std::uint32_t>::max( );


        for( auto & element : constants ) if( element.value( ) > limitation )
        {
            large.push_back( & element );

            tab_large = std::max( tab_large, element.name( ).length( ) );
        }
        else
        {
            small.push_back( & element );

            tab_small = std::max( tab_small, element.name( ).length( ) );
        }

        std::stringstream tempo;
        std::string       value;

        if( small.size( ) > 0 )
        {
            stream << "        " << "enum GLenum32 : unsigned int"  << '\n';
            stream << "        " << "{"                             << '\n';

            for( std::size_t i = 0; auto point : small )
            {
                tempo = std::stringstream( );

                tempo << std::hex << std::uppercase << std::right;
                tempo << std::setw( 8 ) << std::setfill( '0' );
                tempo << point->value( );

                value = tempo.str( );


                stream << std::setw( 12 ) << ' ';
                stream << std::setw( tab_small ) << std::left;
                stream << point->name( ) << " = 0x";

                for( int i = 0; i < 8; ++i )
                {
                    if( i != 0 and i % 4 == 0 )
                    {
                        stream << '\'';
                    }

                    stream << value[ i ];
                }

                if( i != small.size( ) - 1 ) stream <<  ',' << '\n';
                else                         stream << '\n';

                i += 1;
            }

            stream << "        " << "};\n\n";
        }

        if( large.size( ) > 0 )
        {
            stream << "        " << "enum GLenum64 : unsigned long long int" << '\n';
            stream << "        " << "{"                                      << '\n';

            for( std::size_t i = 0; auto point : large )
            {
                tempo = std::stringstream( );

                tempo << std::hex << std::uppercase << std::right;
                tempo << std::setw( 16 ) << std::setfill( '0' );
                tempo << point->value( );

                value = tempo.str( );


                stream << std::setw( 12 ) << ' ';
                stream << std::setw( tab_large );
                stream << point->name( ) << " = 0x";

                for( int i = 0; i < 16; ++i )
                {
                    if( i != 0 and i % 4 == 0 )
                    {
                        stream << '\'';
                    }

                    stream << value[ i ];
                }

                if( i != large.size( ) - 1 ) stream <<  ',' << '\n';
                else                         stream << '\n';

                i += 1;
            }

            stream << "        " << "};\n\n";
        }
    }

    void context::append_functions( std::ostream & stream )
    {
        for( auto & element : functions )
        {
            stream << std::setw( 8 ) << ' ';
            stream << element.declaration( ) << ";\n";
        }
    }
}