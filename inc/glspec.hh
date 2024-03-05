#ifndef MOO_GLSPEC_HH
#define MOO_GLSPEC_HH

#include <string>
#include <vector>


namespace moo
{
    using ordinal = unsigned int;

    enum  profile : unsigned int
    {
        NONE,
        CORE,
        COMPATIBLE
    };


    class context
    {
    public:
       ~context( );
        context( const context & other );
        context( context &&      other );

        context & operator=( const context & other );
        context & operator=( context &&      other );

        static
        context parse( const std::string & filename, ordinal major, ordinal minor, profile type );

        static
        bool    check_command_line( int argc, char * argv[], ordinal & major, ordinal & minor, profile & type, std::string & inputs );

    private:
        context( );

        class constant;
        class function;


        ordinal major;
        ordinal minor;
        profile type;

        std::vector<constant> constants;
        std::vector<function> functions;
    };
}


#endif