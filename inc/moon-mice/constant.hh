#ifndef MOON_MICE_CONSTANT_HH
#define MOON_MICE_CONSTANT_HH

#include <string_view>
#include <vector>
#include <cstdint>
#include <pugixml\pugixml.hh>


namespace moonmice
{
    struct constant
    {
        std::string_view label;
        std::uint64_t    value;

        static void append( pugi::xml_node list, pugi::xml_node root );
        static void remove( pugi::xml_node list );

        static std::string declare_constants_32( );
        static std::string declare_constants_64( );
    };


    extern std::vector<constant> constants;
}


#endif