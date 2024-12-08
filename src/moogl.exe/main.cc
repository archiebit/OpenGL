#include <pugixml\pugixml.hh>

#include <moon-mice\constant.hh>
#include <moon-mice\context.hh>


int main( int argc, char * argv[] )
{
    using namespace moonmice;


    context::select_settings( argc, argv );
    context::append( );


    return 0;
}