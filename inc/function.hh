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

        string &       name( );
        const string & name( ) const;
        string context_name( ) const;


        string declaration( )    const;
        string point( )          const;
        void   implementation( std::ostream & stream, const string & version ) const;

        void append_output( string && type );
        void append_params( string && type, string && name );

        static void types_in_use( listing & integral, listing & floating, listing & versatile );

    private:
        serial  tag;
        serial  out;
        serials types;
        serials names;
    };
}


#endif