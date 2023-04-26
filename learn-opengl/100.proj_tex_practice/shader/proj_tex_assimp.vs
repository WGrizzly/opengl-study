#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 FragNorm;
out vec2 TexCoords;
out vec4 ProjTexCoord;
out mat4 PjtProjMat;

uniform mat4 cam_model;
uniform mat4 cam_view;
uniform mat4 cam_proj;

uniform mat4 pjt_view;
uniform mat4 pjt_proj;


void main()
{
    TexCoords = aTexCoords;

    FragPos = aPos;
    FragNorm = aNormal;
    ProjTexCoord = pjt_proj * pjt_view * vec4(aPos, 1.0);
    PjtProjMat = pjt_proj;

    gl_Position = cam_proj * cam_view * cam_model * vec4(aPos, 1.0);
}