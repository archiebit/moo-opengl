#ifndef MOO_DEFINE_HH
#define MOO_DEFINE_HH

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <cstdint>


namespace moo
{
    using usize   = std::size_t;
    using uint    = std::uint64_t;
    using serial  = std::uint32_t;
    using string  = std::string;

    using table   = std::unordered_map<serial, string>;
    using dict    = std::unordered_map<string, string>;
    using listing = std::unordered_set<string>;

    using strings  = std::vector<string>;
    using serials  = std::vector<serial>;
}


#endif