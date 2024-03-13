#ifndef MOO_PARSE_HH
#define MOO_PARSE_HH

#include <context.hh>

#include <string>
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
    private:
        using node      = rapidxml::xml_node<char>;
        using attribute = rapidxml::xml_attribute<char>;
        using document  = rapidxml::xml_document<char>;

    public:
       ~parse( );
        parse( );

        void init( );
        void exit( );

        void load( std::istream & source );
        void save( int major, int minor, bool compatible );

    private:
        const node * version( int major, int minor );
        const node * require( const node * in, bool compatible, bool next = false );
        const node * removes( const node * in, bool compatible, bool next = false );


        const node * find_constant( const node * want );
        const node * find_function( const node * want );

        void save_constant( const node * info );
        void save_function( const node * info );

        void drop_constant( const node * info );
        void drop_function( const node * info );


        char     * buff;
        document * data;

        const node * regis;
        const node * enums;
        const node * comms;
        const node * feats;
        const node * root;
    };
}


#endif