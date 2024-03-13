#ifndef MOO_FUNCTION_HH
#define MOO_FUNCTION_HH

#include <context.hh>

#include <string>
#include <cstdint>


namespace moo
{
    class context::function
    {
    public:
       ~function( );
        function( std::string && name, std::string && outs, std::vector<std::string> && type, std::vector<std::string> && args );

        std::string & name( );

        std::string declaration( );
        std::string point( );

    private:
        void save_outs( std::string && outs );
        void save_type( std::string && type );
        void save_name( std::string && name );
        void save_tags( std::string && name );

        std::uint32_t              tag;
        std::uint32_t              out;
        std::vector<std::uint32_t> types;
        std::vector<std::uint32_t> names;

        static std::vector<std::string> tags_all;
        static std::vector<std::string> outs_all;
        static std::vector<std::string> type_all;
        static std::vector<std::string> name_all;
    };
}


#endif