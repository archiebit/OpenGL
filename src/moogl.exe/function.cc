#include <iostream>

#include <moon-mice\function.hh>


namespace moonmice
{
    void function::fix_type( std::string & type )
    {
        std::size_t npos = std::string::npos;
        std::size_t    i = 0;

        for( i = type.find( '*' ); i != npos; i = type.find( '*', i ) )
        {
            bool beg = i == 0;
            bool end = i == type.size( ) - 1;

            if( not beg and type[ i - 1 ] != ' ' and type[ i - 1 ] != '*' )
            {
                type.insert( std::next( type.cbegin( ), i ), ' ' );

                i += 1;

                continue;
            }

            if( not end and type[ i + 1 ] != ' ' and type[ i + 1 ] != '*' )
            {
                type.insert( std::next( type.cbegin( ), i + 1 ), ' ' );

                i += 2;

                continue;
            }

            break;
        }

        for( i = type.find( "void" ); i != npos; i = type.find( "void", i + 6 ) )
        {
            type.insert( std::next( type.cbegin( ), i ), { 'G', 'L' } );
        }

        while( type.length( ) and type.back( ) == ' ' )
        {
            type.pop_back( );
        }

        while( type.length( ) and type.front( ) == ' ' )
        {
            type.erase( type.cbegin( ) );
        }
    }
}

namespace moonmice
{
    void function::append( pugi::xml_node list, pugi::xml_node root )
    {
        for( auto want : list.children( "command" ) )
        {
            std::string_view want_name = want.attribute( "name" ).as_string( );
            std::string_view curr_name;
            pugi::xml_node        node;

            for( auto pack : root.children( "commands" ) )
            for( auto curr : pack.children( "command"  ) )
            {
                want_name = want.attribute( "name" ).as_string( );
                curr_name = curr.child( "proto" ).child( "name" ).child_value( );

                if( want_name == curr_name )
                {
                    node = curr;

                    goto found;
                }
            }


        error:
            std::cerr << "ERROR: Invalid XML data!\n";
            std::exit( 1 );


        found:
            std::string              funname;
            std::string              funtype;
            std::vector<std::string> argname;
            std::vector<std::string> argtype;

            for( auto part : node.child( "proto" ).children( ) )
            {
                if( std::strncmp( part.name( ), "name", 4 ) == 0 )
                {
                    funname = part.child_value( );
                    fix_type( funtype );

                    break;
                }
                else if( part.type( ) == pugi::xml_node_type::node_element )
                {
                    funtype.append( part.child_value( ) );
                }
                else
                {
                    funtype.append( part.value( ) );
                }
            }


            for( auto arg : node.children( "param" ) )
            {
                std::string name;
                std::string type;

                for( auto part : arg.children( ) )
                {
                    if( std::strncmp( part.name( ), "name", 4 ) == 0 )
                    {
                        name.append( part.child_value( ) );

                        break;
                    }
                    else if( part.type( ) == pugi::xml_node_type::node_element )
                    {
                        type.append( part.child_value( ) );
                    }
                    else
                    {
                        type.append( part.value( ) );
                    }
                }

                fix_type( type );

                argname.emplace_back( std::move( name ) );
                argtype.emplace_back( std::move( type ) );
            }


            function element
            {
                .function_name = std::move( funname ),
                .function_type = std::move( funtype ),
                .argument_name = std::move( argname ),
                .argument_type = std::move( argtype )
            };

            functions.emplace_back( std::move( element ) );
        }
    }

    void function::remove( pugi::xml_node list )
    {
        for( auto want : list.children( "command" ) )
        {
            auto predicate = [ & ]( function const & element )
            {
                std::string_view want_name = want.attribute( "name" ).as_string( );
                std::string_view elem_name = element.function_name;

                return want_name == elem_name;
            };

            auto starts = functions.begin( );
            auto ending = functions.end( );
            auto target = std::find_if( starts, ending, predicate );

            if( target != ending )
            {
                functions.erase( target );
            }
        }
    }


    std::string   function::declare( )
    {
        std::size_t size_type = 0;
        std::size_t size_name = 0;

        for( auto const & element : functions )
        {
            size_type = std::max( size_type, element.function_type.length( ) );
            size_name = std::max( size_name, element.function_name.length( ) );
        }


        std::size_t step_type = 0;
        std::size_t step_name = 0;
        std::string      outs;

        for( auto const & element : functions )
        {
            step_type = size_type - element.function_type.length( );
            step_name = size_name - element.function_name.length( );

            outs.append( element.function_type ).append( step_type, ' ' );
            outs.append( 1, ' ' );
            outs.append( step_name, ' ' );
            outs.append( element.function_name );


            std::size_t count = element.argument_type.size( );
            std::size_t index;

            outs.append( 1, '(' );

            for( index = 0; index < count; ++index )
            {
                outs.append( 1, ' ' ).append( element.argument_type[ index ] );
                outs.append( 1, ' ' ).append( element.argument_name[ index ] );

                if( index != count - 1 ) outs.append( 1, ',' );
            }

            outs.append( 1, ' ' ).append( 1, ')'  );
            outs.append( 1, ';' ).append( 1, '\n' );
        }


        return outs;
    }

    std::string function::implement( )
    {
        return { };
    }
}

namespace moonmice
{
    std::vector<function> functions;
}