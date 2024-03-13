#include <iostream>
#include <fstream>
#include <iomanip>

#include <context.hh>


using namespace moo;



int main( int argc, char * argv[] )
{
    std::ifstream file( "D:/Programing projects/moo-opengl/res/gl.xml" );
    std::ofstream outs( "gl.hh", outs.trunc );

    context con( file, 3, 2, false );

    con.save_constants( std::cout );

    return 0;
}