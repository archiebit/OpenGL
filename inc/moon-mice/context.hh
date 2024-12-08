#ifndef MOON_MICE_CONTEXT_HH
#define MOON_MICE_CONTEXT_HH

#include <cstdint>
#include <vector>
#include <string>
#include <pugixml\pugixml.hh>


namespace moonmice
{
    class context
    {
    public:
        using ordinal = std::uint_least32_t;
        enum  profile
        {
            NOTHING    = 0,
            CORE       = 1,
            COMPATIBLE = 2
        };


    public:
        static void select_settings( int argc, char * argv[] );

        static void append( );

        static pugi::xml_node root( );
        static pugi::xml_node find( ordinal major, ordinal min );


    private:
        static ordinal    major;
        static ordinal    minor;
        static profile    group;
        static std::string  out;

        static std::vector<std::byte> blob;
        static pugi::xml_document     data;
    };
}


#endif