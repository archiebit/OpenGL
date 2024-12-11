#ifndef MOON_MICE_FUNCTION_HH
#define MOON_MICE_FUNCTION_HH

#include <string_view>
#include <vector>
#include <cstdint>
#include <pugixml\pugixml.hh>


namespace moonmice
{
    struct function
    {
        std::string_view              function_name;
        std::string_view              function_type;
        std::vector<std::string_view> argument_type;
        std::vector<std::string_view> argument_name;
    };
}


#endif