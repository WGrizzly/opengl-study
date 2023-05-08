#include "../includes/shader_m.h"
#include "../includes/camera.h"
#include "../includes/model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <vector>

#include "../includes/self_defines.hpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);
unsigned int load_texture(const char *path);
unsigned int load_texture_clamp_boarder(char const * path);


const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera cam(glm::vec3(0.0f, 0.0f, 5.0f));
Camera pjt1(glm::vec3(0.0f, 0.0f, 5.0f));
Camera pjt2(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool first_mouse = true;
bool light_toggle = false;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

struct vert_info
{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 uv;
};

struct vert_info_test
{
    glm::vec3 pos;
    glm::vec3 norm;
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "bowl texture projection test", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initailize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    std::string bowl_shader_vs_path(BASE_PATH);     bowl_shader_vs_path += "102.proj_tex_bowl/shader/bowl.vs";
    std::string bowl_shader_fs_path(BASE_PATH);     bowl_shader_fs_path += "102.proj_tex_bowl/shader/bowl.fs";
    Shader bowl_shader(bowl_shader_vs_path.c_str(), bowl_shader_fs_path.c_str());

    std::vector<vert_info> vec_vertex_bowl;
    std::vector<unsigned int> vec_indice;
    size_t bowl_vertice_size = 0;
    {
        Model model_obj("/media/dyjeon/db61bdae-f47f-444e-b54e-9628cbdf4ae8/sx-resources/model-3d/bowl-model-1.stl");
        for(auto mesh : model_obj.meshes)
        {
            for(auto vert : mesh.vertices)
            {
                vert_info vi;
                vi.pos  = vert.Position;
                vi.norm = vert.Normal;
                vi.uv.x = 0.f;
                vi.uv.y = 0.f;

                vec_vertex_bowl.push_back(vi);
                bowl_vertice_size += mesh.vertices.size();
            }

            for(auto idx : mesh.indices)
                vec_indice.push_back(idx);
        }
    }


    unsigned int bowlVAO, bowlVBO, bowlEBO;
    glGenVertexArrays(1, &bowlVAO);
    glGenBuffers(1, &bowlVBO);
    glGenBuffers(1, &bowlEBO);

    glBindVertexArray(bowlVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bowlVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_info) * vec_vertex_bowl.size(), &vec_vertex_bowl.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bowlEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vec_indice.size(), &vec_indice.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    


    stbi_set_flip_vertically_on_load(true);
    std::string pjt_map_path(RESOURCE_PATH);    pjt_map_path += "sx-logo-white.jpg";
    unsigned int pjt_map = load_texture_clamp_boarder(pjt_map_path.c_str());

    
    const float aspect_ratio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    const float pjt_fov = glm::radians(20.f);
    glm::mat4 pjt_proj = glm::perspective(
                                        pjt_fov, 
                                        1.f, //aspect_ratio,
                                        0.1f,
                                        100.f);
    glm::mat4 bias_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    bias_mat = glm::scale(bias_mat, glm::vec3(0.5f));

    // projector setting for test
    {
        pjt1.ProcessMouseMovement(0, 150);
        pjt2.ProcessMouseMovement(0, 150);
        pjt2.rotateYaw(-60.f);
    }

    bowl_shader.use();
    //fragment shader
    bowl_shader.setInt  ("pjtTexture"    , 0);
    bowl_shader.setFloat("pjtFOV"        , pjt_fov);
    bowl_shader.setVec3 ("pjtPos1"       , pjt1.Position);
    bowl_shader.setVec3 ("pjtFront1"     , pjt1.Front);
    bowl_shader.setVec3 ("pjtPos2"       , pjt2.Position);
    bowl_shader.setVec3 ("pjtFront2"     , pjt2.Front);
    //vertex shader
    bowl_shader.setMat4 ("pjtView1"      , pjt1.GetViewMatrix());
    bowl_shader.setMat4 ("pjtProjection1", bias_mat * pjt_proj);
    bowl_shader.setMat4 ("pjtView2"      , pjt2.GetViewMatrix());
    bowl_shader.setMat4 ("pjtProjection2", bias_mat * pjt_proj);
    
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        process_input(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pjt_map);

        // view, projection transformations
        glm::mat4 cam_proj = glm::perspective(
                                        glm::radians(cam.Zoom), 
                                        aspect_ratio, 
                                        0.1f,
                                        100.f);
        glm::mat4 cam_view = cam.GetViewMatrix();

        
        bowl_shader.use();
        // bowl_shader.setMat4("pjtView", pjt.GetViewMatrix());

        glm::mat4 bowl_model = glm::mat4(1.0f);
        // bowl_model = glm::translate(bowl_model, glm::vec3(0.f, 0.f, 0.f));
        // bowl_model = glm::scale(bowl_model, glm::vec3(1.2f, 1.2f, 1.2f));
        bowl_shader.setMat4("model", bowl_model);
        bowl_shader.setMat4("camView", cam_view);
        bowl_shader.setMat4("camProj", cam_proj);
        glBindVertexArray(bowlVAO);
        glDrawElements(GL_TRIANGLES, vec_indice.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &bowlVAO);
    glDeleteBuffers(1, &bowlVBO);
    glDeleteBuffers(1, &bowlEBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam.ProcessKeyboard(UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) //GLFW_KEY_LEFT_CONTROL
        cam.ProcessKeyboard(DOWN, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.rotateYaw(0.3);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam.rotateYaw(-0.3);
    // if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    //     light_toggle = !light_toggle;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // std::cout << "x: " << xoffset << ", y: " << yoffset << std::endl;
    cam.ProcessMouseMovement(xoffset, yoffset);
    // pjt.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int load_texture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int load_texture_clamp_boarder(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
