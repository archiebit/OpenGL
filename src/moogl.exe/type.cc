#include <moon-mice\type.hh>


namespace moonmice
{
    std::string type::define( )
    {
        return
        {
            "#ifdef _WIN32\n"
            "    using GLbyte      = signed char;\n"
            "    using GLshort     = signed short int;\n"
            "    using GLint       = signed int;\n"
            "    using GLint64     = signed long long int;\n"
            "\n"
            "    using GLubyte     = unsigned char;\n"
            "    using GLushort    = unsigned short int;\n"
            "    using GLuint      = unsigned int;\n"
            "    using GLuint64    = unsigned long long int;\n"
            "\n"
            "    using GLboolean   = bool;\n"
            "    using GLvoid      = void;\n"
            "    using GLchar      = char;\n"
            "    using GLbitfield  = unsigned int;\n"
            "    using GLsizei     = unsigned int;\n"
            "    using GLenum      = GLenum32;\n"
            "    using GLintptr    = signed long long int;\n"
            "    using GLsizeiptr  = unsigned long long int;\n"
            "    enum  GLsync      : unsigned long long int { };\n"
            "\n"
            "    using GLfloat     = float;\n"
            "    using GLdouble    = double;\n"
            "    using GLclampf    = float;\n"
            "    using GLclampd    = double;\n"
            "    enum  GLhalf      : signed short int { };\n"
            "    enum  GLfixed     : signed int       { };\n"
            "    using GLDEBUGPROC = GLvoid( * )( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const * message, GLvoid const * userParam );\n"
            "#else\n"
            "#  error Target OS is unsupported!\n"
            "#endif\n"
        };
    }
}