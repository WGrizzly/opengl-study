#include "sb7.h"
#include "shader_m.h"
#include <iostream>

using namespace std;

class my_app : public sb7::application
{
public:
    void startup()
    {
        std::string base_path = "/home/dyjeon/developes/learn-opengl/opengl-study/super-bible/chap2/";
        std::string vs_path = base_path + "vertex_shader.vs";
        std::string fs_path = base_path + "fragment_shader.fs";
        // rendering_program = compile_shader();
        shader_program.init(vs_path.c_str(), fs_path.c_str());
        glGenVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
    }
    void render(double curr_time)
    {
        // GLfloat color[] = {
        //     static_cast<float>(std::sin(curr_time))* 0.5f + 0.5f,
        //     static_cast<float>(std::cos(curr_time))* 0.5f + 0.5f,
        //     // 0.f, 0.f,
        //     0.f, 1.f
        // };
        GLfloat color[] = { 0.f, 0.f, 0.f, 1.f };
        glClearBufferfv(GL_COLOR, 0, color);

        shader_program.use();

        GLfloat attrib[] = {
            static_cast<float>(std::sin(curr_time) * 0.5f),
            static_cast<float>(std::cos(curr_time) * 0.6f),
            0.f, 0.f
        };
        glVertexAttrib4fv(0, attrib);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    void shutdown()
    {
        shader_program.delete_program();
        glDeleteVertexArrays(1, &vertex_array_object);
    }

private:
    // GLuint rendering_program;
    Shader shader_program;
    GLuint vertex_array_object;
};
DECLARE_MAIN(my_app);
