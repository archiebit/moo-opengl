#ifndef MOO_CONSTANT_HH
#define MOO_CONSTANT_HH

#include <define.hh>
#include <context.hh>


namespace moo
{
    class context::constant
    {
    public:
       ~constant( );
        constant( const constant & other );
        constant( constant &&      other );
        constant( string && name, uint value );

        constant & operator=( const constant & other );
        constant & operator=( constant &&      other );

        string & name( );
        uint &  value( );

    private:
        string tag;
        uint   val;
    };
}


#endif