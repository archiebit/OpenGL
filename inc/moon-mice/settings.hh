#ifndef MOON_MICE_SETTINGS_HH
#define MOON_MICE_SETTINGS_HH

#include <string>
#include <vector>


namespace moonmice
{
    enum profile
    {
        NONE       = 0,
        CORE       = 1,
        COMPATIBLE = 2
    };


    extern int     major;
    extern int     minor;
    extern profile group;

    extern std::string             output;
    extern std::vector<std::byte> content;


    void check_command_line( int argc, char * argv[] );
}


#endif