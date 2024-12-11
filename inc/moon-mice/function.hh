#ifndef MOON_MICE_FUNCTION_HH
#define MOON_MICE_FUNCTION_HH

#include <string>
#include <vector>
#include <cstdint>
#include <pugixml\pugixml.hh>


namespace moonmice
{
    struct function
    {
        std::string              function_name;
        std::string              function_type;
        std::vector<std::string> argument_name;
        std::vector<std::string> argument_type;


        static void append( pugi::xml_node list, pugi::xml_node root );
        static void remove( pugi::xml_node list );

        static std::string   declare( );
        static std::string implement( );

    private:
        static void fix_type( std::string & type );
    };


    extern std::vector<function> functions;
}


#endif