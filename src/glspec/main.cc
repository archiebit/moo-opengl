#include <iostream>
#include <iomanip>
#include <glspec.hh>


using namespace moo;


static void print_usage( );


int main( int argc, char * argv[] )
{
    if( not context::check_command_line( argc, argv ) )
    {
        print_usage( );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}




static void print_usage( )
{
    static const char message[]
    {
        "Usage:\n"
        "    glspec /V:<version> /I:<filename>\n"
        "    glspec /V:<version> /P:{core | compatible} /I:<filename>\n"
        "\n"
        "Options:\n"
        "    /V:<version>           - Generated file will contain specific\n"
        "                             version of OpenGL API.\n"
        "                             Must be in format like /V:<major>.<minor>.\n"
        "                             Examples: /V:1.0 /V:3.2\n"
        "\n"
        "    /P:{core | compatible} - Chooses the profile of the API.\n"
        "                             Unused if for version under 3.2.\n"
        "                             It is necessary for version 3.2 and above.\n"
        "\n"
        "    /I:<filename>          - Input XML file. Must be in format like\n"
        "                             /I:\"<path>\"."
    };

    std::cerr << message << std::endl;
}