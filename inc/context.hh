#ifndef MOO_CONTEXT_HH
#define MOO_CONTEXT_HH

#include <vector>
#include <istream>
#include <ostream>


namespace moo
{
    class context
    {
    protected:
        class constant;
        class function;

    public:
       ~context( );
        context( );
        context( context && other );
        context( std::istream & source, int major, int minor, bool compatible );

        void save_header( std::ostream & target );
        void save_source( std::ostream & target );

        void save_constants( std::ostream & target );

    protected:
        std::vector<constant> constants;
        std::vector<function> functions;
    };
}


#endif