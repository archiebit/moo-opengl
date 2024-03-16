#include <constant.hh>


namespace moo
{
    using constant = context::constant;


    constant::~constant( ) = default;

    constant:: constant( const constant & other ) = default;

    constant:: constant( constant &&      other ) = default;

    constant:: constant( string && name, uint value )
    {
        tag = static_cast<string &&>( name );
        val = value;
    }


    constant & constant::operator=( const constant & other ) = default;

    constant & constant::operator=( constant &&      other ) = default;


    string & constant::name( )
    {
        return tag;
    }

    uint &   constant::value( )
    {
        return val;
    }
}