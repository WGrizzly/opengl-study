#include <glad/glad.h>  //Be sure to include GLAD before GLFW.
#include <GLFW/glfw3.h>
// #include "glad/glad.h"

#include <iostream>
#include <math.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);





    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 
    // float vertices[] = {
    //      0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //      0.0f,  0.5f, 0.0f   // top 
    // };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shader_program);

        float tv = glfwGetTime();
        float gv = static_cast<float>(sin(tv) / 2.0f + 0.5f);
        int vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, gv, 0.0f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    // if (window == NULL)
    // {
        // std::cout << "Failes to create  GLFW window" << std::endl;
        // glfwTerminate();
        // return -1;
    // }
    // glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    // {
        // std::cout << "Failed to initialize GLAD" << std::endl;
        // return -1;
    // }


    // //vertex shader
    // unsigned int vertex_shader_obj = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertex_shader_obj, 1, &vertexShaderSource, NULL);
    // //오브젝트, 코드를 몇 번 넘길것인지, 소스코드, 마지막 문자
    // glCompileShader(vertex_shader_obj);

    // int success;
    // char infoLog[512];
    // glGetShaderiv(vertex_shader_obj, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
        // glGetShaderInfoLog(vertex_shader_obj, 512, NULL, infoLog);
        // std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }

    // //fragment shader
    // GLuint frag_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(frag_shader_obj, 1, &fragmentShaderSource, NULL);
    // glCompileShader(frag_shader_obj);
    // glGetShaderiv(frag_shader_obj, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
        // glGetShaderInfoLog(vertex_shader_obj, 512, NULL, infoLog);
        // std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }


    // //link shaders
    // GLuint shader_program = glCreateProgram();
    // glAttachShader(shader_program, vertex_shader_obj);
    // glAttachShader(shader_program, frag_shader_obj);
    // glLinkProgram(shader_program);

    // //link error
    // glGetProgramiv(shader_program,  GL_LINK_STATUS, &success);
    // if (!success) {
        // glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        // std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    // }
    // //cpu의 셰이더를 지우는듯 하다.
    // glDeleteShader(vertex_shader_obj);
    // glDeleteShader(frag_shader_obj);



    // float vertices[] = {
        // -0.5f, -0.5f, 0.0f,
        //  0.5f, -0.5f, 0.0f,
        //  0.0f,  0.5f, 0.0f
    // };

    // unsigned int VBO;
    // glGenBuffers(1, &VBO);

    // unsigned int VAO;
    // glGenVertexArrays(1, &VAO);
    // glBindVertexArray(VAO);

    // glGenBuffers(GL_ARRAY_BUFFER, &VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*0*/, (void*)0);
    // /*
        // glVertexAttribPointer(
            // 1) 레이아웃 로케이션(페이지 넘버?),
            // 2) size of the vertex attribute,
                // - the vertex attribute is a vec3 so it is composed of 3 values.
            // 3) specifies the type of the data,
            // 4) is data to be normalized?
                // - if inputting integer types, pass GL_TRUE
                // - the integer data is normalized to 0(or -1 for signed data)
            // 5) size of stride
                // - we could’ve also specified the stride as 0 to let OpenGL determine 
                //   the stride (this only works when values are tightly packed).
            // 6) offset of where the position data begins in the buffer.
    // */
//    glEnableVertexAttribArray(0);


    // while(!glfwWindowShouldClose(window))
    // {
        // process_input(window);

        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        // glUseProgram(shader_program);
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfwSwapBuffers(window);
        // glfwPollEvents();
    // }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glfwTerminate();
    // return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    std::cout << "window size changed to " << width << ", " << height << std::endl;
}
void process_input(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}