#include <moon-mice\constant.hh>

#include <iostream>


namespace moonmice
{
    std::vector<constant> constants;


    void constant::append( pugi::xml_node list, pugi::xml_node root )
    {
        for( auto want : list.children( "enum" ) )
        {
            std::string_view want_name;
            std::string_view curr_name;
            pugi::xml_node        node;

            for( auto pack : root.children( "enums" ) )
            for( auto curr : pack.children( "enum"  ) )
            {
                curr_name = curr.attribute( "name" ).value( );
                want_name = want.attribute( "name" ).value( );

                if( curr_name == want_name )
                {
                    node = curr;

                    goto found;
                }
            }


        error:
            std::cerr << "ERROR: Invalid XML data!\n";
            std::exit( 1 );


        found:
            char const * starts = node.attribute( "value" ).value( );
            char *       ending = nullptr;

            constant element
            {
                .label = want_name,
                .value = std::strtoull( starts, & ending, 0 )
            };

            constants.emplace_back( element );
        }
    }

    void constant::remove( pugi::xml_node list )
    {
        for( auto want : list.children( "enum" ) )
        {
            auto predicate = [ & ]( constant const & element )
            {
                return element.label == want.attribute( "name" ).as_string( );
            };


            auto starts = constants.begin( );
            auto ending = constants.end( );

            constants.erase( std::remove_if( starts, ending, predicate ) );
        }
    }
}