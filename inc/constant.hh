#ifndef MOO_CONSTANT_HH
#define MOO_CONSTANT_HH

#include <context.hh>

#include <string>
#include <cstdint>


namespace moo
{
    class context::constant
    {
    public:
       ~constant( );
        constant( const constant & other );
        constant( std::string && name, std::uint64_t value );

        std::string &  name( );
        std::uint64_t value( );

    private:
        std::string   tag;
        std::uint64_t val;
    };
}


#endif