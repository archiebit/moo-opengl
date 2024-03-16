#ifndef MOO_PARSE_HH
#define MOO_PARSE_HH

#include <define.hh>
#include <context.hh>

#include <exception>
#include <rapidxml.hh>


namespace moo
{
    class parse_exception : public std::exception
    {
    public:
       ~parse_exception( ) override;
        parse_exception( );
    };


    class parse : public context
    {
    public:
        using node      = rapidxml::xml_node<char>;
        using attribute = rapidxml::xml_attribute<char>;
        using document  = rapidxml::xml_document<char>;

    public:
       ~parse( );
        parse( );

        void init( );
        void exit( );

        void load( const string & filename );
        void save( int major, int minor, bool compatible );

    private:
        bool version( const node *& feat, int major, int minor );

        const node * find_constant( const node * want );
        const node * find_function( const node * want );

        void parse_constant( const node * in );
        void parse_function( const node * in );

        void erase_constant( const node * in );
        void erase_function( const node * in );


        char       * buff;
        document   * data;
        const node * root;
    };
}


#endif