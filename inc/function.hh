#ifndef MOO_FUNCTION_HH
#define MOO_FUNCTION_HH

#include <define.hh>
#include <context.hh>


namespace moo
{
    class context::function
    {
    public:
       ~function( );
        function( const function & other );
        function( function &&      other );
        function( string && name );

        function & operator=( const function & other );
        function & operator=( function &&      other );

        string & name( );

        string declaration( ) const;
        string point( )       const;

        void append_output( string && type );
        void append_params( string && type, string && name );

    private:
        serial  tag;
        serial  out;
        serials types;
        serials names;
    };
}


#endif