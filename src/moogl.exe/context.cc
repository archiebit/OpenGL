#include <iostream>
#include <fstream>
#include <unordered_map>

#include <moon-mice\context.hh>
#include <moon-mice\constant.hh>
#include <moon-mice\function.hh>


namespace moonmice
{
    void context::select_settings( int argc, char * argv[] )
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


        bool versioned = false;
        bool  profiled = false;
        bool   inputed = false;
        bool  outputed = false;


        for( int i = 1; i < argc; ++i )
        {
            // Process an "--in=" flag.
            if( std::strncmp( argv[ i ], "--in=",       5 ) == 0 )
            {
                if( inputed )
                {
                    std::cerr << "INFO:  New input file is ignored.\n";

                    continue;
                }


                std::ifstream file;
                std::size_t   size;

                try
                {
                    file.exceptions( ~std::ifstream::goodbit );
                    file.open( argv[ i ] + 5, std::ifstream::binary );

                    file.seekg( 0, std::ifstream::end ); size = file.tellg( );
                    file.seekg( 0, std::ifstream::beg );

                    blob.assign( size + 1, std::byte( 0 ) );

                    file.read( reinterpret_cast<char *>( blob.data( ) ), size );
                    file.close( );
                }
                catch( std::exception & )
                {
                    std::cerr << "ERROR: Cannot read an input file.\n";

                    break;
                }

                data.load_string( reinterpret_cast<pugi::char_t *>( blob.data( ) ) );


                inputed = true; continue;
            }


            // Process a "--version=" flag.
            if( std::strncmp( argv[ i ], "--version=", 10 ) == 0 )
            {
                if( versioned )
                {
                    std::cerr << "INFO:  New version is ignored.\n";

                    continue;
                }


                auto count = std::sscanf( argv[ i ], "--version=%u.%u", & major, & minor );

                if( count != 2 )
                {
                    std::cerr << "ERROR: Invalid version format.\n";

                    break;
                }


                versioned = true; continue;
            }


            // Process a "--profile=" flag.
            if( std::strncmp( argv[ i ], "--profile=", 10 ) == 0 )
            {
                if( profiled )
                {
                    std::cerr << "INFO:  New profile is ignored.\n";

                    continue;
                }


                if( std::strncmp( argv[ i ] + 10, "core",        4 ) == 0 )
                {
                    group = CORE;
                }
                if( std::strncmp( argv[ i ] + 10, "compatible", 10 ) == 0 )
                {
                    group = COMPATIBLE;
                }

                if( group == NOTHING )
                {
                    std::cerr << "ERROR: Invalid profile format.";

                    break;
                }


                profiled = true; continue;
            }


            // Process an "--out=" flag.
            if( std::strncmp( argv[ i ], "--out=",      6 ) == 0 )
            {
                if( outputed )
                {
                    std::cerr << "INFO:  New output path is ignored.\n";

                    continue;
                }

                out = argv[ i ] + 6;


                outputed = true; continue;
            }


            // Unknown flag case.
            std::cerr << "ERROR: Unknown option \"" << argv[ i ] << "\"\n";

            break;
        }




        if( not inputed )
        {
            std::cerr << "ERROR: Input file is not provided!\n";
        }

        if( not versioned )
        {
            std::cerr << "ERROR: Version is not provided!\n";
        }

        if( major >= 3 and group == NOTHING )
        {
            std::cerr << "ERROR: Target version is at least 3.0,\n";
            std::cerr << "       profile must be provided!\n";
        }


        if( not inputed or not versioned or ( major >= 3 and group == NOTHING ) )
        {
            std::cerr << "INFO:  Check the usage guide.\n";
            std::cerr << "\n";
            std::cerr << usage;

            std::exit( 1 );
        }
    }
}


namespace moonmice
{
    template <>
    std::string context::implement<context::INI_PROC_LIST>( )
    {
        std::size_t name_size = 0;
        std::size_t type_size = 0;

        for( auto const & item : functions )
        {
            name_size = std::max( name_size, item.function_name.size( ) );
            type_size = std::max( type_size, item.function_type.size( ) );
        }


        std::string list;

        for( auto const & item : functions )
        {
            {
                auto const & type = item.function_type;
                auto const & name = item.function_name;

                std::size_t name_step = name_size - name.size( );
                std::size_t type_step = type_size - type.size( );


                list.append( type ).append( type_step, ' ' );
                list.append( " ( GLAPI * " );
                list.append( name ).append( name_step, ' ' );
                list.append( " )(" );
            }


            auto const & type = item.argument_type;
            auto const & name = item.argument_name;

            for( std::size_t index = 0; index < type.size( ); ++index )
            {
                list.append( 1, ' ' ).append( type[ index ] );
                list.append( 1, ' ' ).append( name[ index ] );

                if( index != type.size( ) - 1 )
                {
                    list.append( 1, ',' );
                }
            }

            list.append( " );\n" );
        }


        return list;
    }

    void context::replace( std::string & target, std::string const & source, std::string const & pattern )
    {
        std::size_t offset = target.find( pattern );

        if( offset == std::string::npos )
        {
            return;
        }


        std::size_t spaces = offset - target.rfind( '\n', offset ) - 1;
        std::size_t starts = 0;
        std::size_t ending = source.find( '\n' );
        std::size_t length = ending - starts + 1;

        target.erase( offset, pattern.size( ) );

        while( ending != std::string::npos )
        {
            if( starts != 0 )
            {
                target.insert( offset, spaces, ' ' ); offset += spaces;
            }

            target.insert( offset, source, starts, length );

            offset = offset + length;
            starts = ending + 1;
            ending = source.find( '\n', starts );
            length = ending - starts + 1;
        }
    }
}


namespace moonmice
{
    pugi::xml_node context::root( )
    {
        return data.child( "registry" );
    }

    pugi::xml_node context::find( ordinal major, ordinal minor )
    {
        auto pack = root( ).children( "feature" );


        if( not pack.empty( ) ) for( auto current : pack )
        {
            auto version = current.attribute( "number" ).as_string( );
            ordinal  maj = 0;
            ordinal  min = 0;

            auto   count = std::sscanf( version, "%u.%u", & maj, & min );

            if( count != 2 )
            {
                std::cerr << "ERROR: Invalid XML data!\n";
                std::exit( 1 );
            }

            if( major == maj and minor == min )
            {
                return current;
            }
        }


        return { };
    }
}


namespace moonmice
{
    void context::append( )
    {
        int maj = 1, target;
        int min = 0, source;


        while( true )
        {
            target = ( major << 16 ) | minor;
            source = ( maj   << 16 ) | min;


            if( source > target )
            {
                break;
            }

            if( auto feature = find( maj, min ); not feature.empty( ) )
            {
                for( auto list : feature.children( ) )
                {
                    if( 0 == std::strcmp( list.name( ), "require" ) )
                    {
                        pugi::xml_attribute profile = list.attribute( "profile" );
                        std::string_view    special;

                        if( not profile.empty( ) )
                        {
                            special = profile.as_string( );

                            bool core = std::strncmp( profile.as_string( ), "core",           4 ) == 0;
                            bool comp = std::strncmp( profile.as_string( ), "compatibility", 13 ) == 0;

                            switch( group )
                            {
                                case CORE:
                                {
                                    if( core )
                                    {
                                        constant::append( list, root( ) );
                                        function::append( list, root( ) );
                                    }

                                    break;
                                }

                                case COMPATIBLE:
                                {
                                    if( comp )
                                    {
                                        constant::append( list, root( ) );
                                        function::append( list, root( ) );
                                    }

                                    break;
                                }

                                case NOTHING:
                                {
                                    constant::append( list, root( ) );
                                    function::append( list, root( ) );

                                    break;
                                }
                            }
                        }
                        else
                        {
                            constant::append( list, root( ) );
                            function::append( list, root( ) );
                        }

                        continue;
                    }


                    if( 0 == std::strcmp( list.name( ), "remove"  ) )
                    {
                        pugi::xml_attribute profile = list.attribute( "profile" );
                        std::string_view    special;

                        if( not profile.empty( ) )
                        {
                            special = profile.as_string( );

                            bool core = std::strncmp( profile.as_string( ), "core",           4 ) == 0;
                            bool comp = std::strncmp( profile.as_string( ), "compatibility", 13 ) == 0;

                            switch( group )
                            {
                                case CORE:
                                {
                                    if( core )
                                    {
                                        constant::remove( list );
                                        function::remove( list );
                                    }

                                    break;
                                }

                                case COMPATIBLE:
                                {
                                    if( comp )
                                    {
                                        constant::remove( list );
                                        function::remove( list );
                                    }

                                    break;
                                }

                                case NOTHING:
                                {
                                    constant::remove( list );
                                    function::remove( list );

                                    break;
                                }
                            }
                        }
                        else
                        {
                            constant::remove( list );
                            function::remove( list );
                        }

                        continue;
                    }


                    std::cerr << "ERROR: Invalid XML data!\n";
                    std::exit( 1 );
                }

                min += 1;
            }
            else
            {
                maj += 1;
                min -= min;
            }
        }
    }

    void context::define( std::string & value )
    {
        std::unordered_map<std::string, std::size_t> match
        {
            { "<MAJ>",              0 },
            { "<MIN>",              0 },
            { "<GROUP>",            0 },
            { "<ENUM32>",           0 },
            { "<ENUM64>",           0 },
            { "<PROCS>",            0 },
            { "<IMP_CONTEXT_PROC>", 0 },
            { "<IMP_CONTEXT_INTL>", 0 },
            { "// INI_PROC_LIST",   0 }
        };


        while( true )
        {
            std::size_t count = match.size( );
            std::size_t ready = 0;

            for( auto & [ name, data ] : match )
            {
                ready += ( data = value.find( name ) ) == std::string::npos;
            }

            if( ready == count )
            {
                break;
            }


            if( match[ "// INI_PROC_LIST" ] != std::string::npos )
            {
                replace( value, implement<INI_PROC_LIST>( ), "// INI_PROC_LIST" );
            }

            if( match[ "<MAJ>" ] != std::string::npos )
            {
                std::size_t starts = match[ "<MAJ>" ];
                std::size_t length = 5;

                value.replace( starts, length, std::to_string( major ) );

                continue;
            }


            if( match[ "<MIN>" ] != std::string::npos )
            {
                std::size_t starts = match[ "<MIN>" ];
                std::size_t length = 5;

                value.replace( starts, length, std::to_string( minor ) );

                continue;
            }


            if( match[ "<GROUP>" ] != std::string::npos )
            {
                std::size_t starts = match[ "<GROUP>" ];
                std::size_t length = 7;

                switch( group )
                {
                    case NOTHING:    value.replace( starts, length,       "NONE" ); break;
                    case CORE:       value.replace( starts, length,       "CORE" ); break;
                    case COMPATIBLE: value.replace( starts, length, "COMPATIBLE" ); break;
                }

                continue;
            }


            if( match[ "<ENUM32>" ] != std::string::npos )
            {
                std::size_t offset = match[ "<ENUM32>" ];
                std::size_t length = 8;


                std::string record = constant::declare_constants_32( );

                std::size_t   tabs = offset - value.rfind( '\n', offset ) - 1;
                std::size_t starts = 0;
                std::size_t ending = record.find( '\n' );

                value.replace( offset, length, "" );

                while( ending != std::string::npos )
                {
                    if( starts != 0 )
                    {
                        value.insert( offset, tabs, ' ' ); offset += tabs;
                    }

                    value.insert( offset, record, starts, ending - starts + 1 );

                    offset = offset + ending + 1 - starts;
                    starts = ending + 1;
                    ending = record.find( '\n', starts );
                }

                continue;
            }


            if( match[ "<ENUM64>" ] != std::string::npos )
            {
                std::size_t offset = match[ "<ENUM64>" ];
                std::size_t length = 8;


                std::string record = constant::declare_constants_64( );

                std::size_t   tabs = offset - value.rfind( '\n', offset ) - 1;
                std::size_t starts = 0;
                std::size_t ending = record.find( '\n' );

                value.replace( offset, length, "" );

                while( ending != std::string::npos )
                {
                    if( starts != 0 )
                    {
                        value.insert( offset, tabs, ' ' ); offset += tabs;
                    }

                    value.insert( offset, record, starts, ending - starts + 1 );

                    offset = offset + ending + 1 - starts;
                    starts = ending + 1;
                    ending = record.find( '\n', starts );
                }

                continue;
            }


            if( match[ "<PROCS>" ] != std::string::npos )
            {
                std::size_t offset = match[ "<PROCS>" ];
                std::size_t length = 7;


                std::string record = function::declare( );

                std::size_t   tabs = offset - value.rfind( '\n', offset ) - 1;
                std::size_t starts = 0;
                std::size_t ending = record.find( '\n' );

                value.replace( offset, length, "" );

                while( ending != std::string::npos )
                {
                    if( starts != 0 )
                    {
                        value.insert( offset, tabs, ' ' ); offset += tabs;
                    }

                    value.insert( offset, record, starts, ending - starts + 1 );

                    offset = offset + ending + 1 - starts;
                    starts = ending + 1;
                    ending = record.find( '\n', starts );
                }

                continue;
            }


            if( match[ "<IMP_CONTEXT_PROC>" ] != std::string::npos )
            {
                std::size_t offset = match[ "<IMP_CONTEXT_PROC>" ];
                std::size_t length = 18;


                std::string record = function::implement( );

                std::size_t   tabs = offset - value.rfind( '\n', offset ) - 1;
                std::size_t starts = 0;
                std::size_t ending = record.find( '\n' );

                value.replace( offset, length, "" );

                while( ending != std::string::npos )
                {
                    if( starts != 0 )
                    {
                        value.insert( offset, tabs, ' ' ); offset += tabs;
                    }

                    value.insert( offset, record, starts, ending - starts + 1 );

                    offset = offset + ending + 1 - starts;
                    starts = ending + 1;
                    ending = record.find( '\n', starts );
                }

                continue;
            }


            if( match[ "<IMP_CONTEXT_INTL>" ] != std::string::npos )
            {
                std::size_t offset = match[ "<IMP_CONTEXT_INTL>" ];
                std::size_t length = 18;


                std::string record = context::implement_internal( );

                std::size_t   tabs = offset - value.rfind( '\n', offset ) - 1;
                std::size_t starts = 0;
                std::size_t ending = record.find( '\n' );

                value.replace( offset, length, "" );

                while( ending != std::string::npos )
                {
                    if( starts != 0 )
                    {
                        value.insert( offset, tabs, ' ' ); offset += tabs;
                    }

                    value.insert( offset, record, starts, ending - starts + 1 );

                    offset = offset + ending + 1 - starts;
                    starts = ending + 1;
                    ending = record.find( '\n', starts );
                }

                continue;
            }
        }
    }


    void context::create_general_header( )
    {
        std::ofstream  file;
        std::string    path = out + "OpenGL.hh";
        std::string content
        {
            "#ifndef MOON_MICE_OPENGL\n"
            "#define MOON_MICE_OPENGL\n"
            "\n"
            "\n"
            "namespace moonmice::OpenGL\n"
            "{\n"
            "    enum  profile\n"
            "    {\n"
            "        NONE       = 0,\n"
            "        CORE       = 1,\n"
            "        COMPATIBLE = 2\n"
            "    };\n"
            "\n"
            "\n"
            "#ifdef _WIN32\n"
            "    using GLbyte   = signed char;\n"
            "    using GLshort  = signed short int;\n"
            "    using GLint    = signed int;\n"
            "    using GLint64  = signed long long int;\n"
            "\n"
            "    using GLubyte  = unsigned char;\n"
            "    using GLushort = unsigned short int;\n"
            "    using GLuint   = unsigned int;\n"
            "    using GLuint64 = unsigned long long int;\n"
            "\n"
            "    using GLfloat  = float;\n"
            "    using GLdouble = double;\n"
            "\n"
            "    using GLclampf = float;\n"
            "    using GLclampd = double;\n"
            "\n"
            "\n"
            "    using GLenum     = unsigned int;\n"
            "    using GLboolean  = bool;\n"
            "    using GLvoid     = void;\n"
            "    using GLchar     = char;\n"
            "    using GLbitfield = unsigned int;\n"
            "    using GLsizei    = unsigned int;\n"
            "    using GLintptr   = signed long long int;\n"
            "    using GLsizeiptr = unsigned long long int;\n"
            "\n"
            "    enum GLsync  : unsigned long long int { };\n"
            "    enum GLfixed : signed int             { };\n"
            "    enum GLhalf  : signed short int       { };\n"
            "\n"
            "    using GLDEBUGPROC = GLvoid( * )( GLenum, GLenum, GLuint, GLenum, GLsizei, GLchar const *, GLvoid const * );\n"
            "#else\n"
            "#   error Target OS is unsupported!\n"
            "#endif\n"
            "\n"
            "\n"
            "    template <unsigned int major, unsigned int minor, profile group>\n"
            "    class context;\n"
            "}\n"
            "\n"
            "\n"
            "#endif\n"
        };


        try
        {
            file.exceptions( ~std::ofstream::goodbit );
            file.open( path, std::ofstream::trunc );

            file.write( content.data( ), content.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: An error occured while writing to a file.\n";
            std::exit( 1 );
        }
    }

    void context::create_special_header( )
    {
        std::ofstream  file;
        std::string    path = out + "OpenGL_<MAJ>.<MIN>.hh";
        std::string content
        {
            "#ifndef MOON_MICE_OPENGL_<MAJ>_<MIN>\n"
            "#define MOON_MICE_OPENGL_<MAJ>_<MIN>\n"
            "\n"
            "#include \"OpenGL.hh\"\n"
            "\n"
            "\n"
            "#ifdef _WIN32\n"
            "#   define GLAPI __stdcall\n"
            "#else\n"
            "#   define GLAPI\n"
            "#endif\n"
            "\n"
            "\n"
            "namespace moonmice::OpenGL\n"
            "{\n"
            "    template <>\n"
            "    class context<<MAJ>, <MIN>, <GROUP>>\n"
            "    {\n"
            "    private:\n"
            "        struct ini\n"
            "        {\n"
            "            // INI_PROC_LIST\n"
            "        };\n"
            "\n"
            "        struct ini commands;\n"
            "\n"
            "\n"
            "    public:\n"
            "       ~context( );\n"
            "        context( void * data );\n"
            "        context( context const &  ) = delete;\n"
            "        context( context &        ) = delete;\n"
            "        context( context const && ) = delete;\n"
            "        context( context &&       );\n"
            "\n"
            "        context & operator=( context const &  ) = delete;\n"
            "        context & operator=( context &        ) = delete;\n"
            "        context & operator=( context const && ) = delete;\n"
            "        context & operator=( context &&       );\n"
            "\n"
            "\n"
            "    public:\n"
            "        <ENUM32>"
            "\n"
            "        <ENUM64>"
            "\n"
            "        <PROCS>"
            "    };\n"
            "}\n"
            "\n"
            "\n"
            "#endif\n"
        };


        define(    path );
        define( content );

        try
        {
            file.exceptions( ~std::ofstream::goodbit );
            file.open( path, std::ofstream::trunc );

            file.write( content.data( ), content.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: An error occured while writing to a file.\n";
            std::exit( 1 );
        }
    }

    void context::create_loading_header( )
    {
        std::ofstream  file;
        std::string    path = out + "OpenGL_init.hh";
        std::string content
        {
            "#ifndef MOON_MICE_OPENGL_INIT_HH\n"
            "#define MOON_MICE_OPENGL_INIT_HH\n"
            "\n"
            "\n"
            "#ifdef _WIN32\n"
            "#   include <windows.h>\n"
            "\n"
            "\n"
            "#   define WGL_CONTEXT_MAJOR_VERSION             0x0000'2091\n"
            "#   define WGL_CONTEXT_MINOR_VERSION             0x0000'2092\n"
            "#   define WGL_CONTEXT_FLAGS                     0x0000'2094\n"
            "#   define WGL_CONTEXT_DEBUG_BIT                 0x0000'0001\n"
            "#   define WGL_CONTEXT_PROFILE_MASK              0x0000'9126\n"
            "#   define WGL_CONTEXT_CORE_PROFILE_BIT          0x0000'0001\n"
            "#   define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x0000'0002\n"
            "\n"
            "\n"
            "\n"
            "\n"
            "LPVOID  LocateContextCommand( LPCSTR Command )\n"
            "{\n"
            "    FARPROC Invalids[]\n"
            "    {\n"
            "        ( FARPROC )(  0 ),\n"
            "        ( FARPROC )(  1 ),\n"
            "        ( FARPROC )(  2 ),\n"
            "        ( FARPROC )(  3 ),\n"
            "        ( FARPROC )( -1 )\n"
            "    };\n"
            "\n"
            "    HMODULE Lib = GetModuleHandleW( L\"OPENGL32.DLL\" );\n"
            "    FARPROC Fun = GetProcAddress( Lib, Command );\n"
            "\n"
            "    for( FARPROC Case : Invalids ) if( Case == Fun )\n"
            "    {\n"
            "        return ( LPVOID )( wglGetProcAddress( Command ) );\n"
            "    }\n"
            "\n"
            "    return ( LPVOID )Fun;\n"
            "}\n"
            "\n"
            "BOOLEAN CreateContextDummy( HWND Window )\n"
            "{\n"
            "    PIXELFORMATDESCRIPTOR Pixel\n"
            "    {\n"
            "        sizeof( PIXELFORMATDESCRIPTOR ),\n"
            "        1,\n"
            "        PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL,\n"
            "        PFD_TYPE_RGBA,\n"
            "        32,\n"
            "        0, 0, 0, 0, 0, 0, 0, 0,\n"
            "        0, 0, 0, 0, 0,\n"
            "        24,\n"
            "        8,\n"
            "        0,\n"
            "        PFD_MAIN_PLANE,\n"
            "        0, 0, 0, 0\n"
            "    };\n"
            "    HMODULE LibGL = NULL;\n"
            "    HDC        DC = NULL;\n"
            "    HGLRC Context = NULL;\n"
            "    INT    Format = 0;\n"
            "\n"
            "\n"
            "    if( LibGL = LoadLibraryW( L\"OPENGL32.DLL\" ); LibGL == NULL )\n"
            "    {\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "    if( DC = GetDC( Window ); DC == NULL )\n"
            "    {\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "    if( Format = ChoosePixelFormat( DC, & Pixel ); Format == 0 )\n"
            "    {\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "    if( SetPixelFormat( DC, Format, & Pixel ) == FALSE )\n"
            "    {\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "    if( Context = wglCreateContext( DC ); Context == NULL )\n"
            "    {\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "\n"
            "    SetPropW( Window, L\"moonmice/context\", Context );\n"
            "    SetPropW( Window, L\"moonmice/command\", NULL    );\n"
            "\n"
            "    SetLastError( ERROR_SUCCESS );\n"
            "\n"
            "    return TRUE;\n"
            "}\n"
            "\n"
            "BOOLEAN CreateContextFinal( HWND Window, INT Major, INT Minor, BOOLEAN Compatible, BOOLEAN Debug )\n"
            "{\n"
            "    if( CreateContextDummy( Window ) == FALSE )\n"
            "    {\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "    using INIT = HGLRC( * )( HDC, HGLRC, INT const * );\n"
            "\n"
            "    HGLRC Context = ( HGLRC )GetPropW( Window, L\"moonmice/context\" );\n"
            "    HDC    Device = GetDC( Window );\n"
            "\n"
            "    wglMakeCurrent( Device, Context );\n"
            "\n"
            "    INIT   Proc = ( INIT )( LocateContextCommand( \"wglCreateContextAttribsARB\" ) );\n"
            "    INT Attribs[]\n"
            "    {\n"
            "        WGL_CONTEXT_MAJOR_VERSION, Major,\n"
            "        WGL_CONTEXT_MINOR_VERSION, Minor,\n"
            "        WGL_CONTEXT_PROFILE_MASK,  0,\n"
            "        WGL_CONTEXT_FLAGS,         0,\n"
            "        0\n"
            "    };\n"
            "\n"
            "    if( Compatible ) Attribs[ 5 ] |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT;\n"
            "    else             Attribs[ 5 ] |= WGL_CONTEXT_CORE_PROFILE_BIT;\n"
            "\n"
            "    if( Debug      ) Attribs[ 7 ] |= WGL_CONTEXT_DEBUG_BIT;\n"
            "\n"
            "\n"
            "    if( Proc == NULL )\n"
            "    {\n"
            "        wglMakeCurrent( NULL, NULL );\n"
            "        wglDeleteContext( Context );\n"
            "        FreeLibrary( GetModuleHandleW( L\"OPENGL32.DLL\" ) );\n"
            "\n"
            "        RemovePropW( Window, L\"moonmice/context\" );\n"
            "        RemovePropW( Window, L\"moonmice/command\" );\n"
            "\n"
            "        SetLastError( ERROR_PROC_NOT_FOUND );\n"
            "\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "\n"
            "    Context = ( * Proc )( Device, NULL, Attribs );\n"
            "\n"
            "    if( Context == NULL )\n"
            "    {\n"
            "        DWORD Code = GetLastError( );\n"
            "\n"
            "        wglMakeCurrent( NULL, NULL );\n"
            "        wglDeleteContext( ( HGLRC )GetPropW( Window, L\"moonmice/context\" ) );\n"
            "        FreeLibrary( GetModuleHandleW( L\"OPENGL32.DLL\" ) );\n"
            "\n"
            "        RemovePropW( Window, L\"moonmice/context\" );\n"
            "        RemovePropW( Window, L\"moonmice/command\" );\n"
            "\n"
            "        SetLastError( Code );\n"
            "\n"
            "        return FALSE;\n"
            "    }\n"
            "\n"
            "\n"
            "    wglMakeCurrent( Device, Context );\n"
            "    wglDeleteContext( ( HGLRC )GetPropW( Window, L\"moonmice/context\" ) );\n"
            "\n"
            "    SetPropW( Window, L\"moonmice/context\", Context );\n"
            "\n"
            "    SetLastError( ERROR_SUCCESS );\n"
            "\n"
            "    return TRUE;\n"
            "}\n"
            "#else\n"
            "#   error Target OS is unsupported!\n"
            "#endif\n"
            "\n"
            "\n"
            "#endif\n"
        };


        try
        {
            file.exceptions( ~std::ofstream::goodbit );
            file.open( path, std::ofstream::trunc );

            file.write( content.data( ), content.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: An error occured while writing to a file.\n";
            std::exit( 1 );
        }
    }

    void context::create_special_source( )
    {
        std::ofstream  file;
        std::string    path = out + "OpenGL_<MAJ>.<MIN>.cc";
        std::string content
        {
            "#include \"OpenGL_<MAJ>.<MIN>.hh\"\n"
            "\n"
            "#ifdef _WIN32\n"
            "#    include <windows.h>\n"
            "#endif\n"
            "\n"
            "\n"
            "#ifdef _WIN32\n"
            "#   define APIENTRY __stdcall\n"
            "#else\n"
            "#   define APIENTRY\n"
            "#endif\n"
            "\n"
            "\n"
            "namespace moonmice::OpenGL\n"
            "{\n"
            "    <IMP_CONTEXT_INTL>"
            "}\n"
            "\n"
            "namespace moonmice::OpenGL\n"
            "{\n"
            "    <IMP_CONTEXT_PROC>"
            "}"
        };


        define(    path );
        define( content );

        try
        {
            file.exceptions( ~std::ofstream::goodbit );
            file.open( path, std::ofstream::trunc );

            file.write( content.data( ), content.size( ) );
            file.close( );
        }
        catch( std::exception & )
        {
            std::cerr << "ERROR: An error occured while writing to a file.\n";
            std::exit( 1 );
        }
    }


    std::string context::implement_internal( )
    {
        std::string list
        {
            "struct context<<MAJ>, <MIN>, <GROUP>>::implementation\n"
            "{\n"
        };

        context::define( list );


        std::size_t type_size = 0;
        std::size_t name_size = 0;

        for( auto const & element : functions )
        {
            type_size = std::max( type_size, element.function_type.length( ) );
            name_size = std::max( name_size, element.function_name.length( ) );
        }


        std::string points;
        std::string loader;

        for( auto const & element : functions )
        {
            std::string occasion;
            std::size_t type_tabs = type_size - element.function_type.length( );
            std::size_t name_tabs = name_size - element.function_name.length( );

            occasion.append( 4, ' ' );
            occasion.append( element.function_type );
            occasion.append( type_tabs, ' ' );
            occasion.append( " ( APIENTRY * " );
            occasion.append( element.function_name );
            occasion.append( name_tabs, ' ' );
            occasion.append( " )" );

            if( element.argument_type.size( ) != 0 )
            {
                for( std::size_t i = 0; i < element.argument_type.size( ); ++i )
                {
                    if( i == 0 )
                    {
                        occasion.append( "( " );
                    }


                    occasion.append( element.argument_type[ i ] );


                    if( i != element.argument_type.size( ) - 1 )
                    {
                        occasion.append( ", " );
                    }
                    else
                    {
                        occasion.append( " );" );
                    }
                }
            }
            else
            {
                occasion.append( "( );" );
            }

            points.append( occasion ).append( 1, '\n' );
        }


        for( auto const & element : functions )
        {
            std::size_t name_tabs = name_size - element.function_name.length( );

            loader.append( 8, ' ' );
            loader.append( element.function_name );
            loader.append( name_tabs, ' ' );
            loader.append( " = reinterpret_cast<decltype( " );
            loader.append( element.function_name );
            loader.append( name_tabs, ' ' );
            loader.append( " )>( ( * proc )( \"" );
            loader.append( element.function_name );
            loader.append( "\"" );
            loader.append( name_tabs, ' ' );
            loader.append( ") );\n" );
        }


        list.append( points );
        list.append( "\n\n" );
        list.append( "    void locate( void *( * proc )( char const name[] ) )\n");
        list.append( "    {\n" );
        list.append( loader );
        list.append( "    }\n" );

        list.append( "};\n" );


        return list;
    }
}

namespace moonmice
{
    context::ordinal context::major = 0;
    context::ordinal context::minor = 0;
    context::profile context::group = NOTHING;

    std::string            context::out;
    std::vector<std::byte> context::blob;
    pugi::xml_document     context::data;
}