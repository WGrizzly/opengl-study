#include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

// Shader sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 cam_proj;
    uniform mat4 cam_view;
    uniform mat4 pjt_view;
    void main() {
        vec4 worldPos = inverse(pjt_view) * vec4(aPos, 1.0);
        vec4 viewPos = cam_view * worldPos;
        gl_Position = cam_proj * viewPos;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Render the frustum as red lines
    }
)";

GLuint compileShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Shader compilation failed\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "Program linking failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

std::vector<glm::vec3> createFrustumVertices(const glm::mat4& projMatrix, float nearDist, float farDist) {
    glm::mat4 invProj = glm::inverse(projMatrix);

    std::vector<glm::vec4> ndcCoords = {
        {-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},  // Near plane
        {-1, -1, 1, 1},  {1, -1, 1, 1},  {1, 1, 1, 1},  {-1, 1, 1, 1}    //
        // Far plane
    };

    std::vector<glm::vec3> frustumVertices;
    frustumVertices.reserve(8);
    for (const auto& ndc : ndcCoords) {
        glm::vec4 viewPos = invProj * ndc;
        viewPos /= viewPos.w;
        frustumVertices.push_back(glm::vec3(viewPos));
    }

    return frustumVertices;
}

void createFrustumVBO(GLuint& VAO, GLuint& VBO, const std::vector<glm::vec3>& frustumVertices) {
    std::vector<unsigned int> indices = {
        0, 1, 1, 2, 2, 3, 3, 0, // Near plane
        4, 5, 5, 6, 6, 7, 7, 4, // Far plane
        0, 4, 1, 5, 2, 6, 3, 7  // Connecting lines
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, frustumVertices.size() * sizeof(glm::vec3), frustumVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

int main() {
    // Initialize GLFW and create a window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Frustum Visualization", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Compile and link shaders
    GLuint shaderProgram = createShaderProgram();

    // Create frustum vertices
    float nearDist = 0.1f;
    float farDist = 100.0f;
    glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, nearDist, farDist);
    std::vector<glm::vec3> frustumVertices = createFrustumVertices(projMatrix, nearDist, farDist);

    // Create vertex buffer and array objects
    GLuint VAO, VBO;
    createFrustumVBO(VAO, VBO, frustumVertices);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear color and depth buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Update the observer camera's view matrix (rotate around the projection camera)
        float radius = 5.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::vec3 cameraPos = glm::vec3(camX, 0.0f, camZ);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        // Set uniform values for the observer camera
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "cam_proj"), 1, GL_FALSE, glm::value_ptr(projMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "cam_view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Set uniform values for the projection camera (identity matrix as an example)
        glm::mat4 pjtViewMatrix = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "pjt_view"), 1, GL_FALSE, glm::value_ptr(pjtViewMatrix));

        // Render the frustum
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
