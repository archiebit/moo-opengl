#include <iostream>
#include <fstream>
#include <iomanip>

#include <context.hh>


using namespace moo;



int main( int argc, char * argv[] )
{
    ( void )argc, ( void )argv;


    context con( "D:/Programing projects/moo-opengl/res/gl.xml", 3, 2, false );

    con.save( );

    return 0;
}