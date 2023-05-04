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
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool first_mouse = true;
bool light_toggle = false;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// lighting
glm::vec3 light_pos(0.3f, 0.5f, 1.0f);
// glm::vec3 light_pos(1.f, 0.f, 0.0f);

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "basic color", NULL, NULL);
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

    std::string cube_vs_path(BASE_PATH);     cube_vs_path += "101.proj_tex_practice/shader/proj_tex_prac.vs";
    std::string cube_fs_path(BASE_PATH);     cube_fs_path += "101.proj_tex_practice/shader/proj_tex_prac.fs";
    Shader cube_shader(cube_vs_path.c_str(), cube_fs_path.c_str());

    std::string bowl_shader_vs_path(BASE_PATH);     bowl_shader_vs_path += "101.proj_tex_practice/shader/bowl.vs";
    std::string bowl_shader_fs_path(BASE_PATH);     bowl_shader_fs_path += "101.proj_tex_practice/shader/bowl.fs";
    Shader bowl_shader(bowl_shader_vs_path.c_str(), bowl_shader_fs_path.c_str());

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    std::vector<vert_info> vec_vertex;
    for(int line = 0; line < 36; line++)
    {
        size_t idx = line * 8;
        vert_info vi;
        vi.pos  = glm::vec3(vertices[idx + 0], vertices[idx + 1], vertices[idx + 2]);
        vi.norm = glm::vec3(vertices[idx + 3], vertices[idx + 4], vertices[idx + 5]);
        vi.uv   = glm::vec2(vertices[idx + 6], vertices[idx + 7]);
        vec_vertex.push_back(vi);
    }

    glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


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

    // unsigned int bowlVBO;
    // glGenBuffers(1, &bowlVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, bowlVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vert_info) * vec_vertex_bowl.size(), &vec_vertex_bowl.front(), GL_STATIC_DRAW);\
    
    // unsigned int bowlVAO;
    // glGenVertexArrays(1, &bowlVAO);
    // glBindVertexArray(bowlVAO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    // glEnableVertexAttribArray(2);
    // glBindVertexArray(0);

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


    // unsigned int cubeVBO;
    // glGenBuffers(1, &cubeVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vert_info) * vec_vertex.size(), &vec_vertex.front(), GL_STATIC_DRAW);

    // unsigned int cubeVAO;
    // glGenVertexArrays(1, &cubeVAO);
    // glBindVertexArray(cubeVAO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    // glEnableVertexAttribArray(2);
    // glBindVertexArray(0);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_info) * vec_vertex.size(), &vec_vertex.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    


    stbi_set_flip_vertically_on_load(true);

    std::string df_map_path(RESOURCE_PATH);    df_map_path += "container2.png";
    unsigned int diffuse_map = load_texture(df_map_path.c_str());

    std::string spcl_map_path(RESOURCE_PATH);   spcl_map_path += "container2_specular.png";
    unsigned int spcl_map = load_texture(spcl_map_path.c_str());

    std::string ems_map_path(RESOURCE_PATH);    ems_map_path += "matrix.jpg";
    unsigned int ems_map = load_texture(ems_map_path.c_str());

    std::string pjt_map_path(RESOURCE_PATH);    pjt_map_path += "sx-logo-white.jpg";
    unsigned int pjt_map = load_texture_clamp_boarder(pjt_map_path.c_str());

    cube_shader.use();
    cube_shader.setInt("material.diffuse", 0);
    cube_shader.setInt("material.specular", 1);
    cube_shader.setInt("material.emission", 2);
    cube_shader.setInt("pjt_tex", 3);
    cube_shader.setFloat("material.shininess", 64.0f);

    // directional light
    cube_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    cube_shader.setVec3("dirLight.ambient", 0.35f, 0.35f, 0.35f);
    cube_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    cube_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    // spotLight
    cube_shader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
    cube_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    cube_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    cube_shader.setFloat("spotLight.constant", 1.0f);
    cube_shader.setFloat("spotLight.linear", 0.09f);
    cube_shader.setFloat("spotLight.quadratic", 0.032f);
    cube_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    cube_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // glm::vec3 pjt_pos = glm::vec3(0.f, 0.f, 5.0f);
    // glm::vec3 pjt_lookAt = glm::vec3(0.f, 0.f, 0.f);
    // glm::vec3 pjt_up = glm::vec3(0.f, 1.f, 0.f);
    // glm::mat4 pjt_mat_view = glm::lookAt(pjt_pos, pjt_lookAt, pjt_up);
    // glm::mat4 pjt_mat_proj = glm::perspective(glm::radians(45.f), 1.0f, 0.2f, 1000.0f);
    // glm::mat4 pjt_mat_bias = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    // pjt_mat_bias = glm::scale(pjt_mat_bias, glm::vec3(0.5f));

    Camera pjt(glm::vec3(0.0f, 0.0f, 5.0f));
    cube_shader.setMat4("pjt_view", pjt.GetViewMatrix());
    
    const float aspect_ratio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    const float pjt_fov = glm::radians(20.f);
    glm::mat4 pjt_proj = glm::perspective(
                                        pjt_fov, 
                                        1.f, //aspect_ratio,
                                        0.1f,
                                        100.f);
    glm::mat4 bias_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    bias_mat = glm::scale(bias_mat, glm::vec3(0.5f));
    cube_shader.setMat4("pjt_projection", bias_mat * pjt_proj);
    cube_shader.setVec3("pjt_pos", 0.0f, 0.0f, 5.0f);


    bowl_shader.use();
    //fragment shader
    bowl_shader.setInt("pjtTexture", 3);
    bowl_shader.setVec3("pjtPos", pjt.Position);
    bowl_shader.setVec3("pjtFront", pjt.Front);
    bowl_shader.setFloat("pjtFOV", pjt_fov);
    //vertex shader
    bowl_shader.setMat4("pjtView", pjt.GetViewMatrix());
    bowl_shader.setMat4("pjtProjection", bias_mat * pjt_proj);
    

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        process_input(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube_shader.use();
        cube_shader.setVec3("viewPos", cam.Position);
        
        // spotLight
        cube_shader.setVec3("spotLight.position", cam.Position);
        cube_shader.setVec3("spotLight.direction", cam.Front);
        // if(light_toggle)
        // {
        //     cube_shader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        //     cube_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        //     cube_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        // }
        // else
        // {
        //     cube_shader.setVec3("spotLight.ambient", 0.f, 0.f, 0.f);
        //     cube_shader.setVec3("spotLight.diffuse", 0.f, 0.f, 0.f);
        //     cube_shader.setVec3("spotLight.specular", 0.f, 0.f, 0.f);
        // }
        

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, spcl_map);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ems_map);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, pjt_map);

        // view, projection transformations
        glm::mat4 cam_proj = glm::perspective(
                                        glm::radians(cam.Zoom), 
                                        aspect_ratio, 
                                        0.1f,
                                        100.f);
        glm::mat4 cam_view = cam.GetViewMatrix();

        glBindVertexArray(cubeVAO);
        cube_shader.setMat4("camProj", cam_proj);
        cube_shader.setMat4("camView", cam_view);
        for (unsigned int i = 1; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            // model = glm::rotate(model, glm::radians(angle),
            //                     glm::vec3(1.0f, 0.3f, 0.5f));
            cube_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, vec_vertex_bowl.size());
            // glBindVertexArray(0);
        }
        // glBindVertexArray(0);

        
        bowl_shader.use();
        glm::mat4 bowl_model = glm::mat4(1.0f);
        // bowl_model = glm::translate(bowl_model, glm::vec3(0.f, 0.f, 0.f));
        // bowl_model = glm::scale(bowl_model, glm::vec3(1.2f, 1.2f, 1.2f));
        bowl_shader.setMat4("model", bowl_model);
        bowl_shader.setMat4("camView", cam_view);
        bowl_shader.setMat4("camProj", cam_proj);
        glBindVertexArray(bowlVAO);
        glDrawElements(GL_TRIANGLES, vec_indice.size(), GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, bowl_vertice_size);
        // glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &bowlVAO);
    glDeleteBuffers(1, &bowlVBO);
    glDeleteBuffers(1, &bowlEBO);

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

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
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
        light_toggle = !light_toggle;
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

    cam.ProcessMouseMovement(xoffset, yoffset);
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
