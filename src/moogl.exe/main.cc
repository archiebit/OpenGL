#include <iostream>
#include <pugixml\pugixml.hh>

#include <moon-mice\constant.hh>
#include <moon-mice\context.hh>


int main( int argc, char * argv[] )
{
    using namespace moonmice;


    context::select_settings( argc, argv );
    context::append( );

    auto list = constant::declare_constants_32( );

    std::size_t starts = 0;
    std::size_t ending = list.find( '\n' );

    do
    {
        std::string_view view( list.data( ) + starts, ending - starts );

        std::clog << view;

        starts = ending;
        ending = list.find( '\n', starts + 1 );
    }
    while( ending != std::string::npos );


    return 0;
}