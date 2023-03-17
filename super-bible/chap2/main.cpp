#include "sb7.h"
#include <iostream>

using namespace std;

GLuint compile_shader(void)
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    const GLchar *vertex_shader_source[] =
    {
        "#version 430 core                          \n"
        "                                           \n"
        "void main()                                \n"
        "{                                          \n"
        "   const vec4 vertices[3] = vec4[3](       \n"
        "       vec4(0.25, -0.25, 0.5, 1.0),        \n"
        "       vec4(-0.25, -0.25, 0.5, 1.0),       \n"
        "       vec4(0.25, 0.25, 0.5, 1.0)          \n"
        "   );                                      \n"
        "   gl_Position = vertices[gl_VertexID];    \n"
        "}                                          \n"
    };

    const GLchar *frag_shader_source[] =
    {
        "#version 430 core                          \n"
        "                                           \n"
        "out vec4 color;                            \n"
        "                                           \n"
        "void main()                                \n"
        "{                                          \n"
        "   color = vec4(1.0f, 0.5f, 0.2f, 1.0f);   \n"
        "}                                          \n"
    };

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, frag_shader_source, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

class my_app : public sb7::application
{
public:
    void startup()
    {
        rendering_program = compile_shader();
        glGenVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
    }
    void render(double curr_time)
    {
        GLfloat color[] = {
            static_cast<float>(std::sin(curr_time))* 0.5f + 0.5f,
            static_cast<float>(std::cos(curr_time))* 0.5f + 0.5f,
            // 0.f, 0.f,
            0.f, 1.f
        };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);
        // glPointSize(40.f);
        // glDrawArrays(GL_POINTS, 0, 3);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    void shutdown()
    {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vertex_array_object);
    }

private:
    GLuint rendering_program;
    GLuint vertex_array_object;
};
DECLARE_MAIN(my_app);
