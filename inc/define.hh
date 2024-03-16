#ifndef MOO_DEFINE_HH
#define MOO_DEFINE_HH

#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>


namespace moo
{
    using usize  = std::size_t;
    using uint   = std::uint64_t;
    using serial = std::uint32_t;
    using string = std::string;

    using table  = std::unordered_map<serial, string>;

    using strings = std::vector<string>;
    using serials = std::vector<serial>;
}


#endif