#include <iostream>
#include <pugixml\pugixml.hh>

#include <moon-mice\constant.hh>
#include <moon-mice\function.hh>
#include <moon-mice\context.hh>
#include <moon-mice\type.hh>



int main( int argc, char * argv[] )
{
    using namespace moonmice;

    context::select_settings( argc, argv );
    context::append( );

    context::create_general_header( );
    context::create_special_header( );
    context::create_special_source( );


    return 0;
}