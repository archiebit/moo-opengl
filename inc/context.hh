#ifndef MOO_CONTEXT_HH
#define MOO_CONTEXT_HH

#include <define.hh>
#include <istream>
#include <ostream>


namespace moo
{
    class context
    {
    public:
        class constant;
        class function;

    public:
       ~context( );
        context( );
        context( context && other );
        context( const string & filename, int major, int minor, bool compatible );


        void save( );

    protected:
        string version( );

        void append_datatypes( std::ostream & stream );
        void append_file_head( std::ostream & stream );

        void class_head( std::ostream & stream );
        void class_foot( std::ostream & stream );

        void append_constants( std::ostream & stream );
        void append_functions( std::ostream & stream );


        int  major;
        int  minor;
        bool compatible;

        std::vector<constant> constants;
        std::vector<function> functions;
    };
}


#endif