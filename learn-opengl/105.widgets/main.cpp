#include "../includes/shader_m.h"
#include "../includes/camera.h"
#include "../includes/model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <vector>
#include <unordered_map>

#include <chrono>
#include <vtkKdTree.h>
#include <vtkOBBTree.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkAutoInit.h>
#include <vtkNew.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)


#include "../includes/self_defines.hpp"
#include "../includes/sx_simple_shader.hpp"

#ifndef CHRONO_NOW_HRES
	#define CHRONO_NOW_HRES std::chrono::high_resolution_clock::now()
#endif
#ifndef CHRONO_ELAPSED_MSEC
	#define CHRONO_ELAPSED_MSEC(a, b) std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count()
#endif

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);
unsigned int load_texture(const char *path);
unsigned int load_texture_clamp_boarder(char const * path, int& cols, int& rows, int& chan);


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

struct Plane
{
    glm::vec3 norm;
    float d;
    Plane(const glm::vec4& a_, const glm::vec4& b_, const glm::vec4 &c_)
    {
        glm::vec3 a = glm::vec3(a_) / a_[3];
        glm::vec3 b = glm::vec3(b_) / b_[3];
        glm::vec3 c = glm::vec3(c_) / c_[3];

        glm::vec3 ab = glm::normalize(b - a);
        glm::vec3 ac = glm::normalize(c - a);
        norm = glm::normalize( glm::cross(ab, ac) );
        // norm = glm::cross(ab, ac);
        d = -glm::dot(norm, a);
    }
    Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        glm::vec3 ab = glm::normalize(b - a);
        glm::vec3 ac = glm::normalize(c - a);
        norm = glm::normalize( glm::cross(ab, ac) );
        // norm = glm::cross(ab, ac);
        d = -glm::dot(norm, a);
    }
};

struct Line
{
    glm::vec3 point;
    glm::vec3 direction;
};

Line get_intersection_line(const Plane& p1, const Plane& p2)
{
    //implemented based on link: https://www.omnicalculator.com/math/line-of-intersection-of-two-planes
    //assume that plane's normal is already normalize.
    glm::vec3 cross = glm::cross(p1.norm, p2.norm);
    if(glm::length(cross) < 1e-6)
    {
        throw std::runtime_error("two plane is parallel");
    }

    float dot = glm::dot(p1.norm, p2.norm);

    float d1 = p1.d;
    float d2 = p2.d;

    float k1 = (d1 - d2* dot) / (1 - dot);
    float k2 = (d2 - d1* dot) / (1 - dot);

    glm::vec3 k1n1 = k1*p1.norm;
    glm::vec3 k2n2 = k2*p2.norm;

    Line intersection_line;
    intersection_line.point = k1n1 + k2n2;
    intersection_line.direction = cross;
    return intersection_line;
}

Line intersectPlanes(const Plane& p1, const Plane& p2)
{
    // Line direction is cross product of the plane normals
    glm::vec3 line_dir = glm::cross(p1.norm, p2.norm);

    // Line direction should be normalized
    line_dir = glm::normalize(line_dir);

    // We solve the system of equations to find a point on the line
    glm::mat3 A(p1.norm, p2.norm, line_dir);
    glm::vec3 b(-p1.d, -p2.d, 0);

    // The solution is a point on the line
    glm::vec3 line_point = glm::inverse(A) * b;

    // The result is the line of intersection
    return Line {line_point, line_dir};
}

Line intersect_line(const Plane& p1, const Plane& p2)
{
    //https://stackoverflow.com/questions/6408670/line-of-intersection-between-two-planes

    glm::vec3 p3_normal = glm::cross(p1.norm, p2.norm);
    float det = glm::length(p3_normal);
    if(det < 1e-6)
    {
        throw std::runtime_error("two plane is parallel");
    }

    glm::vec3 pt = ((glm::cross(p3_normal, p2.norm) * p1.d) + 
                    (glm::cross(p1.norm, p3_normal) * p2.d)) / det;
    glm::vec3 norm = p3_normal;

    Line inter_line;
    inter_line.point = pt;
    inter_line.direction = norm;
    return inter_line;
}

/*
    Up, Down, Left, Right order

         7_______________ 6
         /|             /|
        / |            / |
       /  |           /  |
      /   |          /   |
   3 /____|_________/    |
    |     |        | 2   |
    |     |________|_____|
    |    / 4       |    / 5
    |   /          |   /
    |  /           |  /
    | /            | /
    |/_____________|/
   0               1

   returns up, down, left, right
*/
std::vector<Plane> calc_side_planes(const std::vector<glm::vec4>& pts)
{
    assert(pts.size() == 8);

    //each plane's normal direction is inside of frustum
    std::vector<Plane> rv;
    rv.push_back( Plane(pts[2], pts[6], pts[7]) );
    rv.push_back( Plane(pts[1], pts[5], pts[4]) );
    rv.push_back( Plane(pts[0], pts[4], pts[7]) );
    rv.push_back( Plane(pts[1], pts[2], pts[6]) );
    return rv;
}

glm::mat4 rotationMatrixFromVectors(glm::vec3 a, glm::vec3 b)
{
    float cosTheta = glm::dot(a, b); // Cosine of the angle
    glm::vec3 rotationAxis = glm::cross(a, b); // Rotation axis

    if (glm::length(rotationAxis) < 1e-6)
    {
        // Vectors are parallel
        rotationAxis = glm::vec3(1.0, 0.0, 0.0); // Default axis if cross product is zero
    }
    else
    {
        rotationAxis = glm::normalize(rotationAxis); // Normalize the axis
    }

    float angle = acos(cosTheta); // Angle in radians

    // Create a rotation matrix around rotation axis by the angle
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);

    return rotationMatrix;
}

glm::mat4 get_rmat(glm::vec3 from, glm::vec3 to)
{
    from = glm::normalize(from);
    to = glm::normalize(to);

    glm::vec3 axis = glm::cross(from, to);

    float angle = acos(glm::dot(from, to));

    glm::mat4 rmat = glm::rotate(glm::mat4(1.0f), angle, axis);
    return rmat;
}

bool plane_ray_intersect(glm::vec3& contact, glm::vec3 ray, glm::vec3 ray_org, vtkPoints* points)
{
    if (points->GetNumberOfPoints() < 3)
    {
        return false; // Not enough points to define a plane
    }

    // Extract three points from vtkPoints to define the plane
    double p0[3], p1[3], p2[3];
    points->GetPoint(0, p0);
    points->GetPoint(1, p1);
    points->GetPoint(2, p2);

    // Compute plane normal
    glm::vec3 a(p0[0], p0[1], p0[2]);
    glm::vec3 b(p1[0], p1[1], p1[2]);
    glm::vec3 c(p2[0], p2[1], p2[2]);
    glm::vec3 plane_normal = glm::normalize(glm::cross(b - a, c - a));

    // Plane equation: Ax + By + Cz + D = 0; D can be computed as -dot(plane_normal, a)
    float D = -glm::dot(plane_normal, a);

    // Check if the ray and plane are parallel
    float denom = glm::dot(plane_normal, ray);
    if (0.1e-06 > std::fabs(denom - 0.0f))
    {
        return false; // No intersection, the line is parallel to the plane
    }

    // Compute intersection
    float x = -(glm::dot(plane_normal, ray_org) + D) / denom;
    contact = ray_org + glm::normalize(ray) * x;

    return true;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

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
    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(1);

    Model model_obj("/media/dyjeon/db61bdae-f47f-444e-b54e-9628cbdf4ae8/sx-resources/model-3d/bowl-model-1.stl");

    vtkNew<vtkOBBTree> obb_tree;
    vtkNew<vtkKdTree> kd_tree;
    vtkNew<vtkCellArray> cell_list;
    vtkNew<vtkPolyData> poly;
    {
        vtkNew<vtkSTLReader> reader;
        reader->SetFileName("/media/dyjeon/db61bdae-f47f-444e-b54e-9628cbdf4ae8/sx-resources/model-3d/bowl-model-1.stl");
        reader->Update();

        poly->DeepCopy(reader->GetOutput());
        std::cout << "# of pts: " << poly->GetNumberOfPoints() << std::endl;

        obb_tree->SetDataSet(poly);
        obb_tree->BuildLocator();
        obb_tree->Update();

        kd_tree->BuildLocatorFromPoints(poly->GetPoints());
    }

    
    

    std::string bowl_shader_vs_path(BASE_PATH);     bowl_shader_vs_path += "105.widgets/shader/bowl.vs";
    std::string bowl_shader_fs_path(BASE_PATH);     bowl_shader_fs_path += "105.widgets/shader/bowl.fs";
    // Shader bowl_shader(bowl_shader_vs_path.c_str(), bowl_shader_fs_path.c_str());
    sx_simple_shader bowl_shader_;
    bowl_shader_.compile_shader(bowl_shader_vs_path);
    bowl_shader_.compile_shader(bowl_shader_fs_path);
    bowl_shader_.link();


    std::string frustum_shader_vs_path(BASE_PATH);    frustum_shader_vs_path += "105.widgets/shader/frustum.vs";
    std::string frustum_shader_fs_path(BASE_PATH);    frustum_shader_fs_path += "105.widgets/shader/frustum.fs";
    // Shader frustum_shader(frustum_shader_vs_path.c_str(), frustum_shader_fs_path.c_str());
    sx_simple_shader frustum_shader_;
    frustum_shader_.compile_shader(frustum_shader_vs_path);
    frustum_shader_.compile_shader(frustum_shader_fs_path);
    frustum_shader_.link();

    std::string line_shader_vs_path(BASE_PATH);    line_shader_vs_path += "105.widgets/shader/line.vs";
    std::string line_shader_fs_path(BASE_PATH);    line_shader_fs_path += "105.widgets/shader/line.fs";
    sx_simple_shader line_shader;
    line_shader.compile_shader(line_shader_vs_path);
    line_shader.compile_shader(line_shader_fs_path);
    line_shader.link();
    
    

    std::vector<glm::vec4> vec_frustum_ndc_pt = {
        { -1.0f, -1.0f,  1.0f, 1.0f },    // FBL
        {  1.0f, -1.0f,  1.0f, 1.0f },    // FBR
        {  1.0f,  1.0f,  1.0f, 1.0f },    // FTR
        { -1.0f,  1.0f,  1.0f, 1.0f },    // FTL
        { -1.0f, -1.0f, -0.0f, 1.0f },    // NBL
        {  1.0f, -1.0f, -0.0f, 1.0f },    // NBR
        {  1.0f,  1.0f, -0.0f, 1.0f },    // NTR
        { -1.0f,  1.0f, -0.0f, 1.0f }     // NTL
    };
    // for(auto& pt : vec_frustum_ndc_pt)//for testing
    // {
    //     pt[0] /= 2.;
    //     pt[1] /= 2.;
    // }
    std::vector<unsigned int> vec_frustum_idx = {
        //near plane
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        //far plane
        4, 5,
        5, 6,
        6, 7,
        7, 4,

        //connect plane
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };

    std::vector<glm::vec4> vec_line_ndc_pt = {
        { -0.25f, 0.0f, 0.0f, 1.0f },
        {  0.25f, 0.0f, 0.0f, 1.0f }
    };
    std::vector<unsigned int> vec_line_idx = { 0, 1};

    stbi_set_flip_vertically_on_load(true);
    // std::string pjt_map1_path(RESOURCE_PATH);    pjt_map1_path += "kass-3.jpg";
    std::string pjt_map1_path(RESOURCE_PATH);    pjt_map1_path += "sx-logo-white.jpg";
    int tex1_cols, tex1_rows, tex1_chan;
    unsigned int pjt_map1 = load_texture_clamp_boarder(pjt_map1_path.c_str(), tex1_cols, tex1_rows, tex1_chan);
    std::cout << "projector texture1 size: " << tex1_cols << ", " << tex1_rows << ", " << tex1_chan << std::endl;

    // std::string pjt_map2_path(RESOURCE_PATH);    pjt_map2_path += "kass-0.jpg";
    std::string pjt_map2_path(RESOURCE_PATH);    pjt_map2_path += "sx-logo-white.jpg";
    int tex2_cols, tex2_rows, tex2_chan;
    unsigned int pjt_map2 = load_texture_clamp_boarder(pjt_map2_path.c_str(), tex2_cols, tex2_rows, tex2_chan);
    std::cout << "projector texture2 size: " << tex2_cols << ", " << tex2_rows << ", " << tex2_chan << std::endl;

    
    const float aspect_ratio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    const float pjt_fov = glm::radians(20.f);
    const float pjt_ar = static_cast<float>(tex1_cols) / static_cast<float>(tex1_rows);   //projector aspect ratio
    const float pjt_near = 0.1f;
    const float pjt_far = 50.f;
    
    glm::mat4 pjt_proj = glm::perspective( pjt_fov, pjt_ar, pjt_near, pjt_far);
    glm::mat4 bias_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    bias_mat = glm::scale(bias_mat, glm::vec3(0.52f));
    // std::cout << "projector aspect ratio: " << pjt_ar << std::endl;
    // std::cout << bias_mat[0][0] << ", " << bias_mat[1][0] << ", " << bias_mat[2][0] << ", " << bias_mat[3][0] << std::endl
    //           << bias_mat[0][1] << ", " << bias_mat[1][1] << ", " << bias_mat[2][1] << ", " << bias_mat[3][1] << std::endl
    //           << bias_mat[0][2] << ", " << bias_mat[1][2] << ", " << bias_mat[2][2] << ", " << bias_mat[3][2] << std::endl
    //           << bias_mat[0][3] << ", " << bias_mat[1][3] << ", " << bias_mat[2][3] << ", " << bias_mat[3][3] << std::endl; 


    // projector setting for test
    {
        pjt1.ProcessMouseMovement(0, 150);
        pjt2.ProcessMouseMovement(0, 150);
        pjt2.rotateYaw(-60.f);
    }

    // 프로젝터가 움직이지 않는다고 가정했을때만 사용
    //                                  vtkIdType for point id in poly
    std::map<double, std::map<double, glm::vec3>> pjtn_map;
    {
        //initialize map
        
        glm::mat4 transform_mat = bias_mat * pjt_proj * pjt1.GetViewMatrix() * glm::mat4(1.0f);

        for(vtkIdType pt_id = 0; pt_id < poly->GetNumberOfPoints(); pt_id++)
        {
            double *pt = poly->GetPoint(pt_id);
            glm::vec3 gl_pt;
            gl_pt[0] = static_cast<float>(pt[0]);
            gl_pt[1] = static_cast<float>(pt[1]);
            gl_pt[2] = static_cast<float>(pt[2]);

            glm::vec4 uv = transform_mat * glm::vec4(gl_pt, 1.0);
            uv[0] /= uv[3];
            uv[1] /= uv[3];
            uv[2] /= uv[3];
            uv[3] /= uv[3];

            if(1.0 > uv[0] && uv[0] >= 0.0)
            {

            }
        }
    }


    bowl_shader_.use();
    //fragment shader
    bowl_shader_.setInt  ("pjtTexture0"   , 0);
    bowl_shader_.setInt  ("pjtTexture1"   , 1);
    bowl_shader_.setFloat("pjtFOV"        , pjt_fov);
    bowl_shader_.setVec3 ("pjtPos1"       , pjt1.Position);
    bowl_shader_.setVec3 ("pjtFront1"     , pjt1.Front);
    bowl_shader_.setVec3 ("pjtPos2"       , pjt2.Position);
    bowl_shader_.setVec3 ("pjtFront2"     , pjt2.Front);
    //vertex shader
    bowl_shader_.setMat4 ("pjtProjection1", bias_mat * pjt_proj);
    bowl_shader_.setMat4 ("pjtProjection2", bias_mat * pjt_proj);
    

    bool direct_test = false;
    unsigned int frustumVAO, frustumVBO, frustumEBO;
    {
        if(direct_test)
        {
            glm::mat4 im = glm::inverse(pjt_proj * pjt1.GetViewMatrix());
            for(size_t c = 0; c < vec_frustum_ndc_pt.size(); c++)
            {
                glm::vec4 world_pt = im * vec_frustum_ndc_pt[c];
                vec_frustum_ndc_pt[c] = world_pt;
                // std::cout << "world pt at (" << c << "), " << world_pt[0] << ", "
                // << world_pt[1] << ", "<< world_pt[2] << ", "<< world_pt[3] << std::endl;
            }
        }

        glGenVertexArrays(1, &frustumVAO);
        glGenBuffers(1, &frustumVBO);
        glGenBuffers(1, &frustumEBO);

        glBindVertexArray(frustumVAO);
        glBindBuffer(GL_ARRAY_BUFFER, frustumVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vec_frustum_ndc_pt.size(), &vec_frustum_ndc_pt.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frustumEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vec_frustum_idx.size(), &vec_frustum_idx.front(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);        
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    frustum_shader_.use();
    frustum_shader_.setMat4("pjt_proj", pjt_proj);
    frustum_shader_.setBool("direct_test", direct_test);

    unsigned int lineVAO, lineVBO, lineEBO;
    {
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);
        glGenBuffers(1, &lineEBO);

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vec_line_ndc_pt.size(), &vec_line_ndc_pt.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vec_line_idx.size(), &vec_line_idx.front(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);        
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    line_shader.use();
    line_shader.setMat4("pjt_proj", pjt_proj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pjt_map1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pjt_map2);

    float delta_sum = 0.f;
    int frame_cnt = 0;
    while (!glfwWindowShouldClose(window))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        if(delta_sum >= 1.0f)
        {
            std::cout << "fps: " << frame_cnt << std::endl;
            delta_sum = 0.f;
            frame_cnt = 0;
        }
        else
        {
            delta_sum += delta_time;
            frame_cnt++;
        }

        process_input(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // view, projection transformations
        glm::mat4 cam_proj = glm::perspective(
                                        glm::radians(cam.Zoom), 
                                        aspect_ratio, 
                                        0.1f,
                                        100.f);
        glm::mat4 cam_view = cam.GetViewMatrix();

        
        bowl_shader_.use();
        bowl_shader_.setMat4("pjtView1", pjt1.GetViewMatrix());
        bowl_shader_.setMat4("pjtView2", pjt2.GetViewMatrix());

        glm::mat4 bowl_model = glm::mat4(1.0f);
        // bowl_model = glm::translate(bowl_model, glm::vec3(0.f, 0.f, 0.f));
        // bowl_model = glm::scale(bowl_model, glm::vec3(1.2f, 1.2f, 1.2f));
        bowl_shader_.setMat4("model", bowl_model);
        bowl_shader_.setMat4("camView", cam_view);
        bowl_shader_.setMat4("camProj", cam_proj);

        std::vector<glm::vec4> vec_frustum_world_pt1, vec_frustum_world_pt2;
        if (!direct_test)
        {
            glm::mat4 im1 = glm::inverse(pjt_proj * pjt1.GetViewMatrix());
            for (size_t c = 0; c < vec_frustum_ndc_pt.size(); c++)
            {
                glm::vec4 ndc_pt = vec_frustum_ndc_pt[c];
                glm::vec4 world_pt = im1 * ndc_pt;
                vec_frustum_world_pt1.push_back(world_pt);
            }

            glm::mat4 im2 = glm::inverse(pjt_proj * pjt2.GetViewMatrix());
            for(size_t c = 0; c < vec_frustum_ndc_pt.size(); c++)
            {
                glm::vec4 ndc_pt = vec_frustum_ndc_pt[c];
                glm::vec4 world_pt = im2 * ndc_pt;
                vec_frustum_world_pt2.push_back(world_pt);
            }
        }
        else
        {
            for(size_t c = 0; c < vec_frustum_ndc_pt.size(); c++)
                vec_frustum_world_pt1.push_back(vec_frustum_ndc_pt[c]);
        }

        std::vector<Plane> vec_side_planes_1 = calc_side_planes(vec_frustum_world_pt1);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[0].norm", vec_side_planes_1[0].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[0].d",    vec_side_planes_1[0].d);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[1].norm", vec_side_planes_1[1].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[1].d",    vec_side_planes_1[1].d);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[2].norm", vec_side_planes_1[2].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[2].d",    vec_side_planes_1[2].d);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[3].norm", vec_side_planes_1[3].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[3].d",    vec_side_planes_1[3].d);
        std::vector<Plane> vec_side_planes_2 = calc_side_planes(vec_frustum_world_pt2);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[4].norm", vec_side_planes_2[0].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[4].d",    vec_side_planes_2[0].d);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[5].norm", vec_side_planes_2[1].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[5].d",    vec_side_planes_2[1].d);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[6].norm", vec_side_planes_2[2].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[6].d",    vec_side_planes_2[2].d);
        bowl_shader_.setVec3 ("pjtFrustumPlanes[7].norm", vec_side_planes_2[3].norm);
        bowl_shader_.setFloat("pjtFrustumPlanes[7].d",    vec_side_planes_2[3].d);
        {
            const Plane& frust1_right = vec_side_planes_1[3];
            const Plane& frust1_down  = vec_side_planes_1[1];
            const Plane &frust2_left = vec_side_planes_2[2];
            const Plane &frust2_down = vec_side_planes_2[1];

            // Line up_line = get_intersection_line(frust1_right, frust2_left);
            // Line up_line = intersectPlanes(frust1_right, frust2_left);
            Line up_line = intersect_line(frust1_right, frust2_left);
            bowl_shader_.setVec3("up_line.point", up_line.point);
            bowl_shader_.setVec3("up_line.direction", up_line.direction);

            // Line down_line = get_intersection_line(frust1_down, frust2_down);
            // Line down_line = intersectPlanes(frust1_down, frust2_down);
            Line down_line = intersect_line(frust1_down, frust2_down);
            bowl_shader_.setVec3("down_line.point", down_line.point);
            bowl_shader_.setVec3("down_line.direction", down_line.direction);

            Plane blend_plane(down_line.point, up_line.point, up_line.point + up_line.direction * 1.f);
            bowl_shader_.setVec3("pjtBlendPlane.norm", blend_plane.norm);
            bowl_shader_.setFloat("pjtBlendPlane.d", blend_plane.d);
        }
        model_obj.Draw(bowl_shader_);

        frustum_shader_.use();
        glBindVertexArray(frustumVAO);
        frustum_shader_.setMat4("cam_proj", cam_proj);
        frustum_shader_.setMat4("cam_view", cam_view);
        frustum_shader_.setMat4("pjt_proj", pjt_proj);
        frustum_shader_.setMat4("pjt_view", pjt1.GetViewMatrix());
        glLineWidth(2.0f);
        glDrawElements(GL_LINES, vec_frustum_idx.size(), GL_UNSIGNED_INT, 0);
        GLenum glErr;
        int retCode = 0;

        glErr = glGetError();
        while (glErr != GL_NO_ERROR)
        {
            const char *message = "";
            switch (glErr)
            {
            case GL_INVALID_ENUM:
                message = "Invalid enum";
                break;
            case GL_INVALID_VALUE:
                message = "Invalid value";
                break;
            case GL_INVALID_OPERATION:
                message = "Invalid operation";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                message = "Invalid framebuffer operation";
                break;
            case GL_OUT_OF_MEMORY:
                message = "Out of memory";
                break;
            default:
                message = "Unknown error";
            }

            printf("glError in file %s @ line %d: %s\n", "main", 0, message);
            retCode = 1;
            glErr = glGetError();
        }

        frustum_shader_.setMat4("pjt_view", pjt2.GetViewMatrix());
        glDrawElements(GL_LINES, vec_frustum_idx.size(), GL_UNSIGNED_INT, 0);

        glm::mat4 hm = glm::inverse(cam.GetViewMatrix()) * pjt2.GetViewMatrix();

        {
            auto _tp1 = CHRONO_NOW_HRES;

            for (int c = 100; c < 100; c++)
            {
                glm::vec3 gl_pt_start = pjt2.Position;
                glm::vec3 gl_pt_end = pjt2.Position + pjt2.Front * 1000.f;
                double ray_start[3] = {gl_pt_start[0], gl_pt_start[1], gl_pt_start[2]};
                double ray_end[3] = {gl_pt_end[0], gl_pt_end[1], gl_pt_end[2]};

                // intersection test
                vtkNew<vtkPoints> intersection_pts;
                obb_tree->IntersectWithLine(ray_start, ray_end, intersection_pts, nullptr);

                double inter_pt1[3];
                intersection_pts->GetPoint(0, inter_pt1);

                gl_pt_start = pjt1.Position;
                gl_pt_end = pjt1.Position + pjt1.Front * 1000.f;
                ray_start[0] = gl_pt_start[0];
                ray_start[1] = gl_pt_start[1];
                ray_start[2] = gl_pt_start[2];
                ray_end[0] = gl_pt_end[0];
                ray_end[1] = gl_pt_end[1];
                ray_end[2] = gl_pt_end[2];

                double inter_pt2[3];
                obb_tree->IntersectWithLine(ray_start, ray_end, intersection_pts, nullptr);
                intersection_pts->GetPoint(0, inter_pt2);
            }

            for(int c = 0; c < 1000; c++)
            {
                double search_pt[3] = {1.1, 2.2, 3.3};
                double dist = 0.f;

                vtkIdType id = kd_tree->FindClosestPoint(search_pt, dist);

                vtkNew<vtkIdList> cell_id_list;
                poly->GetPointCells(id, cell_id_list);
                // std::cout << "# of cells:" << cell_id_list->GetNumberOfIds() << std::endl;

                double ray1_start[3], ray1_end[3];
                glm::vec3 gl_pt_start = pjt2.Position;
                glm::vec3 gl_pt_end = pjt2.Position + pjt2.Front * 1000.f;
                {
                    ray1_start[0] = gl_pt_start[0];
                    ray1_start[1] = gl_pt_start[1];
                    ray1_start[2] = gl_pt_start[2];

                    ray1_end[0] = gl_pt_end[0];
                    ray1_end[1] = gl_pt_end[1];
                    ray1_end[2] = gl_pt_end[2];
                }

                for(vtkIdType i = 0; i < cell_id_list->GetNumberOfIds(); i++)
                {
                    vtkIdType cell_id = cell_id_list->GetId(i);
                    vtkSmartPointer<vtkCell> cell = poly->GetCell(cell_id);
                    vtkSmartPointer<vtkPoints> pts = cell->GetPoints();

                    glm::vec3 contact;
                    if(plane_ray_intersect(contact, pjt2.Position, pjt2.Front, pts))
                    {
                        break;
                    }
                }
            }

            auto _tp2 = CHRONO_NOW_HRES;
            // std::cout << "tree searching time: " << CHRONO_ELAPSED_MSEC(_tp1, _tp2) << std::endl;

            if(false)
            {
                glGenVertexArrays(1, &lineVAO);
                glGenBuffers(1, &lineVBO);
                glGenBuffers(1, &lineEBO);

                glBindVertexArray(lineVAO);
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vec_line_ndc_pt.size(), &vec_line_ndc_pt.front(), GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vec_line_idx.size(), &vec_line_idx.front(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }
        }

        line_shader.use();
        glBindVertexArray(lineVAO);
        line_shader.setMat4("cam_proj", cam_proj);
        line_shader.setMat4("cam_view", cam_view);
        line_shader.setMat4("pjt_proj", pjt_proj);
        line_shader.setMat4("pjt_view", pjt2.GetViewMatrix());
        line_shader.setMat4("hm", hm);
        glLineWidth(2.0f);
        glDrawElements(GL_LINES, vec_frustum_idx.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &frustumVAO);
    glDeleteBuffers(1, &frustumVBO);
    glDeleteBuffers(1, &frustumEBO);

    glfwTerminate();
    return 0;
}

void save_image(const char *filepath, GLFWwindow *w)
{
    int width, height;
    glfwGetFramebufferSize(w, &width, &height);

    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;

    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
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
    {
        pjt1.rotateYaw(0.3);
        pjt2.rotateYaw(0.3);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        pjt1.rotateYaw(-0.3);
        pjt2.rotateYaw(-0.3);
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        std::cout << "save buffer as image." << std::endl;
        save_image("/media/dyjeon/db61bdae-f47f-444e-b54e-9628cbdf4ae8/sx-resources/outputs/3d-blend-test.png", window);
    }
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

unsigned int load_texture_clamp_boarder(char const * path, int& cols, int& rows, int& chan)
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

        cols = width;
        rows = height;
        chan = nrComponents;

        stbi_image_free(data);
    }
    else
    {
        cols = -1;
        rows = -1;
        chan = -1;

        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
