#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 FragNorm;
out vec4 ProjTexCoord;

uniform mat4 cam_model;
uniform mat4 cam_view;
uniform mat4 cam_proj;

uniform mat4 pjt_view;
uniform mat4 pjt_proj;


void main()
{
    FragPos = vec3(cam_model * vec4(aPos, 1.0));
    FragNorm = mat3(transpose(inverse(cam_model))) * aNormal;  
    ProjTexCoord = pjt_proj * pjt_view * vec4(FragPos, 1.0);

    gl_Position = cam_proj * cam_view * vec4(FragPos, 1.0);
}