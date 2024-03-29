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
void processInput(GLFWwindow* window);
unsigned int load_texture_clamp_boarder(char const * path);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// Camera cam(glm::vec3(0.0f, 20.0f, 20.0f));
Camera cam(glm::vec3(0.0f, 0.0f, 5.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

struct vert_info
{
    glm::vec3 vertice;
    glm::vec3 normal;
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Loading", NULL, NULL);
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
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader shader(
        "/home/dyjeon/developes/learn-opengl/opengl-study/learn-opengl/100.proj_tex_practice/shader/proj_tex_assimp.vs",
        "/home/dyjeon/developes/learn-opengl/opengl-study/learn-opengl/100.proj_tex_practice/shader/proj_tex_assimp.fs"
    );

    // Model model_obj("/home/dyjeon/developes/learn-opengl/LearnOpenGL-master/resources/objects/nanosuit/nanosuit.obj");
    // Model model_obj("/home/dyjeon/developes/learn-opengl/LearnOpenGL-master/resources/objects/backpack/backpack.obj");
    Model model_obj("/media/dyjeon/db61bdae-f47f-444e-b54e-9628cbdf4ae8/sx-resources/model-3d/bowl-model-1.stl");

    std::vector<vert_info> verts;
    for(auto mesh : model_obj.meshes)
    {
        for(auto vert : mesh.vertices)
        {
            vert_info vi;
            vi.vertice = vert.Position;
            vi.normal = vert.Normal;
            verts.push_back(vi);
        }
    }

    unsigned int bowlVBO;
    glGenBuffers(1, &bowlVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bowlVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts) * verts.size(), &verts.front(), GL_STATIC_DRAW);

    unsigned int bowlVAO;
    glGenVertexArrays(1, &bowlVAO);
    glBindVertexArray(bowlVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    if(false)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0); 
    }


    Camera pjt(glm::vec3(0.0f, 0.0f, 5.0f));
    shader.setMat4("pjt_view", pjt.GetViewMatrix());    //vs

    const float aspect_ratio = 1.f;
    glm::mat4 pjt_proj = glm::perspective(
        glm::radians(5.f),
        aspect_ratio,
        0.1f,
        100.f
    );

    glm::mat4 bias_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    bias_mat = glm::scale(bias_mat, glm::vec3(0.5f));
    shader.setMat4("pjt_proj", bias_mat * pjt_proj);   //vs
    shader.setVec3("pjt_pos", glm::vec3(0.0f, 0.0f, 5.0f)); //fs

    std::string pjt_map_path(RESOURCE_PATH);    pjt_map_path += "sx-logo-white.jpg";
    stbi_set_flip_vertically_on_load(true);
    unsigned int pjt_map = load_texture_clamp_boarder(pjt_map_path.c_str());
    shader.setInt("pjt_tex", 0);


    //draw in wire-frame
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window))
    {
        float curr_frame = static_cast<float>(glfwGetTime());
        delta_time = curr_frame - last_frame;
        last_frame = curr_frame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pjt_map);

        glm::mat4 cam_proj = glm::perspective(
            glm::radians(cam.Zoom), 
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
            0.1f, 100.f
        );
        glm::mat4 view = cam.GetViewMatrix();
        shader.setMat4("cam_proj", cam_proj);
        shader.setMat4("cam_view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setMat4("cam_model", model);

        glBindVertexArray(bowlVAO);
        glDrawArrays(GL_TRIANGLES, 0, verts.size());

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    std::cout << "model loading end." << std::endl;
    return 0;
}


void processInput(GLFWwindow *window)
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
}

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

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    cam.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.ProcessMouseScroll(static_cast<float>(yoffset));
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
