#include <iostream>
#include <pugixml\pugixml.hh>

#include <moon-mice\constant.hh>
#include <moon-mice\function.hh>
#include <moon-mice\context.hh>



int main( int argc, char * argv[] )
{
    using namespace moonmice;


    context::select_settings( argc, argv );
    context::append( );

    std::string list = function::declare( );


    std::size_t starts = 0;
    std::size_t ending = list.find( '\n' );

    while( true )
    {
        std::clog << std::string_view( list.data( ) + starts, ending - starts );

        starts = ending;
        ending = list.find( '\n', starts + 1 );

        if( ending == std::string::npos )
        {
            std::clog << '\n'; break;
        }
    }


    return 0;
}