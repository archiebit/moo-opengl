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
    static const char LF = '\n';
}

namespace moo
{
    string context::version( )
    {
        std::stringstream ss;

        ss << '<' << major << ',';
        ss << ' ' << minor << ',';
        ss << ' ';

        if( compatible ) ss << "COMPATIBLE";
        else             ss << "CORE";

        ss << '>';

        return ss.str( );
    }

    void context::append_datatypes( std::ostream & stream )
    {
        listing integral, floating, versatile;

        function::types_in_use( integral, floating, versatile );

        static const dict windows
        {
            { "GLbyte",     "signed char"            },
            { "GLshort",    "signed short int"       },
            { "GLint",      "signed int"             },
            { "GLint64",    "signed long long int"   },

            { "GLubyte",    "unsigned char"          },
            { "GLushort",   "unsigned short int"     },
            { "GLuint",     "unsigned int"           },
            { "GLuint64",   "unsigned long long int" },

            { "GLintptr",   "signed long long int"   },
            { "GLsizeiptr", "unsigned long long int" },


            { "GLfloat",  "float"            },
            { "GLdouble", "double"           },
            { "GLclampf", "float"            },
            { "GLclampd", "double"           },
            { "GLhalf",   "signed short int" },


            { "GLboolean",  "bool"                   },
            { "GLvoid",     "void"                   },
            { "GLchar",     "char"                   },
            { "GLsizei",    "unsigned int"           },
            { "GLbitfield", "unsigned int"           },
            { "GLintptr",   "signed long long int"   },
            { "GLsizeiptr", "unsigned long long int" },
            { "GLenum",     "GLenum32"               },
            { "GLsync",     "unsigned long long int" },
            { "GLfixed",    "signed int"             }
        };


        usize tab;

        if( integral.size( ) != 0 )
        {
            for( tab = 0; const auto & type : integral )
            {
                tab = std::max( tab, type.length( ) );
            }

            stream << LF << LF;
            stream << std::setw( 8 ) << ' ' << "// Integral types."                        << LF;
            stream << std::setw( 4 ) << ' ' << "#if defined _WIN32 and defined __x86_64__" << LF;

            for( const auto & type : integral )
            {
                stream << std::setw( 8 ) << ' ';

                stream << "using " << std::setw( tab ) << type;
                stream << " = "    << windows.at( type );
                stream << ';'      << LF;
            }

            stream << std::setw( 4 ) << ' ' << "#else"                                                        << LF;
            stream << std::setw( 4 ) << ' ' << "#    error Target platform or architecture is not supported!" << LF;
            stream << std::setw( 4 ) << ' ' << "#endif"                                                       << LF;
        }


        if( floating.size( ) != 0 )
        {
            for( tab = 0; const auto & type : floating )
            {
                tab = std::max( tab, type.length( ) );
            }

            stream << LF << LF;
            stream << std::setw( 8 ) << ' ' << "// Floating types."                        << LF;
            stream << std::setw( 4 ) << ' ' << "#if defined _WIN32 and defined __x86_64__" << LF;

            for( const auto & type : floating )
            {
                stream << std::setw( 8 ) << ' ';

                if( type == "GLhalf" )
                {
                    stream << "enum  " << std::setw( tab ) << type;
                    stream << " : "    << windows.at( type );
                    stream << " { };"  << LF;
                }
                else
                {
                    stream << "using " << std::setw( tab ) << type;
                    stream << " = "    << windows.at( type );
                    stream << ';'      << LF;
                }
            }

            stream << std::setw( 4 ) << ' ' << "#else"                                                        << LF;
            stream << std::setw( 4 ) << ' ' << "#    error Target platform or architecture is not supported!" << LF;
            stream << std::setw( 4 ) << ' ' << "#endif"                                                       << LF;
        }


        if( versatile.size( ) != 0 )
        {
            for( tab = 0; const auto & type : versatile )
            {
                tab = std::max( tab, type.length( ) );
            }

            stream << LF << LF;
            stream << std::setw( 8 ) << ' ' << "// Versatile types."                       << LF;
            stream << std::setw( 4 ) << ' ' << "#if defined _WIN32 and defined __x86_64__" << LF;

            for( const auto & type : versatile )
            {
                stream << std::setw( 8 ) << ' ';

                if( type == "GLsync" or type == "GLfixed" )
                {
                    stream << "enum  " << std::setw( tab ) << type;
                    stream << " : "    << windows.at( type );
                    stream << " { };"  << LF;
                }
                else
                {
                    stream << "using " << std::setw( tab ) << type;
                    stream << " = "    << windows.at( type );
                    stream << ';'      << LF;
                }
            }

            stream << std::setw( 4 ) << ' ' << "#else"                                                        << LF;
            stream << std::setw( 4 ) << ' ' << "#    error Target platform or architecture is not supported!" << LF;
            stream << std::setw( 4 ) << ' ' << "#endif"                                                       << LF;
        }
    }

    void context::append_file_head( std::ostream & stream )
    {
        stream << "namespace moo"                                                            << LF;
        stream << "{"                                                                        << LF;
        stream << std::setw( 4 ) << ' ' << "enum profile"                                    << LF;
        stream << std::setw( 4 ) << ' ' << "{"                                               << LF;
        stream << std::setw( 8 ) << ' ' << "CORE,"                                           << LF;
        stream << std::setw( 8 ) << ' ' << "COMPATIBLE"                                      << LF;
        stream << std::setw( 4 ) << ' ' << "};"                                              << LF;
        stream                                                                               << LF;
        stream << std::setw( 4 ) << ' ' << "template <int major, int minor, profile option>" << LF;
        stream << std::setw( 4 ) << ' ' << "class context;"                                  << LF;
        stream << "}"                                                                        << LF;
        stream                                                                               << LF;
    }

    void context::class_head( std::ostream & stream )
    {
        stream << "namespace moo"                                                 << LF;
        stream << "{"                                                             << LF;
        stream << std::setw( 4 ) << ' ' << "template <>"                          << LF;
        stream << std::setw( 4 ) << ' ' << "class context" << version( )          << LF;
        stream << std::setw( 4 ) << ' ' << "{"                                    << LF;
        stream << std::setw( 4 ) << ' ' << "public:"                              << LF;
        stream << std::setw( 7 ) << ' ' << "~context( );"                         << LF;
        stream << std::setw( 8 ) << ' ' << "context( );"                          << LF;
        stream << std::setw( 8 ) << ' ' << "context( const context & ) = delete;" << LF;
        stream << std::setw( 8 ) << ' ' << "context( context &&      ) = delete;" << LF;
    }

    void context::class_foot( std::ostream & stream )
    {
        stream << LF;
        stream << std::setw( 4 ) << ' ' << "private:"                << LF;
        stream << std::setw( 8 ) << ' ' << "struct commands;"        << LF;
        stream << std::setw( 8 ) << ' ' << "struct commands * call;" << LF;
        stream << std::setw( 4 ) << ' ' << "};"                      << LF;
        stream << "}"                                                << LF;
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

        implement_commands( impl );
        implement_functions( impl );


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
            stream << LF;
            stream << std::setw( 8 ) << ' ' << "enum GLenum32" << LF;
            stream << std::setw( 8 ) << ' ' << "{"             << LF;

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

                if( i != small.size( ) - 1 ) stream <<  ',' << LF;
                else                         stream << LF;

                i += 1;
            }

            stream << std::setw( 8 ) << ' ' << "};" << LF;
        }

        if( large.size( ) > 0 )
        {
            stream << LF;
            stream << std::setw( 8 ) << ' ' << "enum GLenum64" << LF;
            stream << std::setw( 8 ) << ' ' << "{"             << LF;

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

            stream << std::setw( 8 ) << ' ' << "};" << LF;
        }
    }

    void context::append_functions( std::ostream & stream )
    {
        stream << LF << LF;

        for( auto & element : functions )
        {
            stream << std::setw( 8 ) << ' ' << element.declaration( );
            stream << ';' << LF;
        }
    }


    void context::implement_commands( std::ostream & stream )
    {
        stream << "#include \"gl.hh\""                                                          << LF;
        stream << ""                                                                            << LF;
        stream << ""                                                                            << LF;

        stream << "#ifdef NDEBUG"                                                               << LF;

        stream << "#   define IMPLEMENT_VOID( name, ... )                                  \\"  << LF;
        stream << "    return ( * call->gl ## name )( __VA_ARGS__ );"                           << LF;
        stream                                                                                  << LF;
        stream << "#   define IMPLEMENT_TYPE( name, ... )                                  \\"  << LF;
        stream << "    return ( * call->gl ## name )( __VA_ARGS__ );"                           << LF;

        stream << "#else"                                                                       << LF;

        stream << "#   define IMPLEMENT_VOID( name, ... )                                  \\"  << LF;
        stream << "    while( ( * call->glGetError )( ) != GL_NO_ERROR );                  \\"  << LF;
        stream << "                                                                        \\"  << LF;
        stream << "    ( * call->gl ## name )( __VA_ARGS__ );                              \\"  << LF;
        stream << "                                                                        \\"  << LF;
        stream << "    if( GLenum code = ( * call->glGetError )( ); code != GL_NO_ERROR )  \\"  << LF;
        stream << "    {                                                                   \\"  << LF;
        stream << "        throw code;                                                     \\"  << LF;
        stream << "    }"                                                                       << LF;
        stream                                                                                  << LF;
        stream << "#   define IMPLEMENT_TYPE( name, ... )                                  \\"  << LF;
        stream << "    while( ( * call->glGetError )( ) != GL_NO_ERROR );                  \\"  << LF;
        stream << "                                                                        \\"  << LF;
        stream << "    auto back = ( * call->gl ## name )( __VA_ARGS__ );                  \\"  << LF;
        stream << "                                                                        \\"  << LF;
        stream << "    if( GLenum code = ( * call->glGetError )( ); code != GL_NO_ERROR )  \\"  << LF;
        stream << "    {                                                                   \\"  << LF;
        stream << "        throw code;                                                     \\"  << LF;
        stream << "    }                                                                   \\"  << LF;
        stream << "    else return back;"                                                       << LF;

        stream << "#endif"                                                                      << LF;
        stream << ""                                                                            << LF;

        stream << "namespace moo"                                     << LF;
        stream << "{"                                                 << LF;
        stream << "    struct context" << version( )  << "::commands" << LF;
        stream << "    {"                                             << LF;

        for( const auto & element : functions )
        {
            stream << std::setw( 8 )   << ' ';
            stream << element.point( ) << ';';
            stream << LF;
        }

        stream << "    };" << LF;
        stream << "}"      << LF;
    }

    void context::implement_functions( std::ostream & stream )
    {
        stream << LF << LF;

        stream << "namespace moo" << LF;
        stream << "{"             << LF;

        for( auto ver = version( ); const auto & element : functions )
        {
            element.implementation( stream, ver );
            stream << LF;
        }

        stream << "}"             << LF;
    }
}