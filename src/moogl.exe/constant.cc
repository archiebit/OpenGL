#include <iostream>
#include <limits>

#include <moon-mice\constant.hh>


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


    std::string constant::declare_constants_32( )
    {
        std::size_t                   size = 0;
        std::string                   outs;
        std::vector<constant const *> list;
        std::array<char, 9>           hexs = { };


        for( std::size_t i = 0; i < constants.size( ); ++i )
        {
            std::uintmax_t maximum = std::numeric_limits<std::uint_least32_t>::max( );
            std::uintmax_t current = constants[ i ].value;

            if( current > maximum )
            {
                continue;
            }

            list.push_back( & constants[ i ] );

            size = std::max( size, constants[ i ].label.length( ) );
        }


        for( std::size_t i = 0; i < list.size( ); ++i )
        {
            if( i == 0 )
            {
                outs.append( "enum GLenum32\n{\n" );
            }


            auto numb = list[ i ]->value;
            auto name = list[ i ]->label;

            auto  tab = 4;
            auto  len = size - name.length( );

            outs.append( tab, ' ' ).append( name );
            outs.append( len, ' ' ).append( " = 0x" );


            for( std::size_t i = 7; i < 8; --i )
            {
                auto digit = numb bitand 0x0F; numb >>= 4;

                if( digit < 10 ) hexs[ i ] = '0' + digit;
                else             hexs[ i ] = 'A' + digit - 10;
            }

            outs.append( hexs.data( ) + 0, 4 ).append( 1, '\'' );
            outs.append( hexs.data( ) + 4, 4 );


            if( i == list.size( ) - 1 )
            {
                outs.append( "\n};\n" );
            }
            else
            {
                outs.append( ",\n" );
            }
        }

        return outs;
    }
}