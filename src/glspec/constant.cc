#include <constant.hh>


namespace moo
{
    context::constant::~constant( )
    { }

    context::constant:: constant( const constant & other ) = default;

    context::constant:: constant( std::string && name, std::uint64_t value )
    {
        tag = static_cast<std::string &&>( name );
        val = value;
    }


    std::string & context::constant::name( )
    {
        return tag;
    }

    std::uint64_t context::constant::value( )
    {
        return val;
    }
}