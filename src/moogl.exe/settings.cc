#include <moon-mice\settings.hh>

#include <iostream>
#include <fstream>


namespace moonmice
{
    static char const usage[]
    {
        "USAGE:                                                                         \n"
        "    moogl --in=<PATH> --version=<MAJ>.<MIN> [options]                          \n"
        "                                                                               \n"
        "ARGUMENTS:                                                                     \n"
        "    --in=<PATH>           : Specifies the path to the registry file            \n"
        "                            in XML format.                                     \n"
        "                                                                               \n"
        "    --out=<PATH>          : Specifies output file path.                        \n"
        "                            The default value is the current directory.        \n"
        "                                                                               \n"
        "    --version=<MAJ>.<MIN> : Specifies OpenGL version.                          \n"
        "                                                                               \n"
        "    --profile=<NAME>      : Specifies OpenGL profile. Possible values are:     \n"
        "                            'core' and 'compatible'.                           \n"
        "                            This flag is necessary if the OpenGL version is 3.0\n"
        "                            or higher.                                         \n"
        "                                                                               \n"
        "EXAMPLE:                                                                       \n"
        "    moogl --in=\"gl.xml\" --version=3.0 --profile=core                         \n"
    };
}
namespace moonmice
{
    int     major = 0;
    int     minor = 0;
    profile group = NONE;

    std::string             output = ".";
    std::vector<std::byte> content;


    void check_command_line( int argc, char * argv[] )
    {
        bool version_exist = false;
        bool   group_exist = false;
        bool   input_exist = false;
        bool  output_exist = false;


        for( int i = 1; i < argc; ++i )
        {
            if( 0 == std::strncmp( argv[ i ], "--in=",      5  ) )
            {
                if( input_exist )
                {
                    std::clog << "The input file has already been defined.\n";

                    continue;
                }

                std::ifstream file;
                std::size_t   size;

                // This is dumb. VS Code can't handle spaces in
                // launch.json 'args' array.
                // So this is my way to provide gl.xml.
                #ifdef DEVELOPMENT
                std::string   path( "D:\\Programing projects\\OpenGL\\.vscode\\gl.xml" );
                #else
                std::string   path( argv[ i ] + 5 );
                #endif


                try
                {
                    file.exceptions( ~file.goodbit );
                    file.open( path, file.binary );

                    file.seekg( 0, std::ifstream::end ); size = file.tellg( );                    
                    file.seekg( 0, std::ifstream::beg );

                    content.assign( size, std::byte( 0 ) );
                    file.read( reinterpret_cast<char *>( content.data( ) ), size );

                    file.close( );
                }
                catch( std::exception & )
                {
                    std::cerr << "An error occurred while reading the file.\n";

                    break;
                }

                input_exist = true;

                continue;
            }



            if( 0 == std::strncmp( argv[ i ], "--version=", 10 ) )
            {
                if( version_exist )
                {
                    std::clog << "The version has already been defined.\n";

                    continue;
                }


                int count = std::sscanf( argv[ i ], "--version=%u.%u", & major, & minor );

                if( count != 2 )
                {
                    std::cerr << "Invalid version format.\n";

                    break;
                }



                version_exist = true;

                continue;
            }



            if( 0 == std::strncmp( argv[ i ], "--profile=", 10 ) )
            {
                if( group_exist )
                {
                    std::clog << "The profile has already been defined.";
                    std::clog << std::endl;

                    continue;
                }


                if( 0 == std::strcmp( argv[ i ], "--profile=core"       ) )
                {
                    group = CORE;
                }
                if( 0 == std::strcmp( argv[ i ], "--profile=compatible" ) )
                {
                    group = COMPATIBLE;
                }

                if( group == NONE )
                {
                    std::cerr << "Invalid profile format.\n";

                    break;
                }


                group_exist = true;

                continue;
            }



            if( 0 == std::strncmp( argv[ i ], "--out=",     6  ) )
            {
                if( output_exist )
                {
                    std::clog << "The output path has already been defined.";
                    std::clog << std::endl;

                    continue;
                }


                output = std::string( argv[ i ] + 6 );


                output_exist = true;

                continue;
            }



            std::cerr << "Unknown option: " << argv[ i ] << "\n";

            break;
        }


        if( not version_exist )
        {
            std::cerr << "Version must be defined.\n";
        }

        if( not input_exist )
        {
            std::cerr << "Input file must be defined.\n";
        }

        if( major >= 3 and group == NONE )
        {
            std::cerr << "Profile is undefined, but version is 3.0 or higher.\n";
        }

        if( not version_exist or not input_exist or ( major >= 3 and group == NONE ) )
        {
            std::cerr << "\n";
            std::cerr << usage << std::endl;

            std::exit( 1 );
        }

        if( not output_exist )
        {
            output = ".";
        }
    }
}