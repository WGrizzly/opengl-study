#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 ProjTexCoord;

//camera info
uniform mat4 model;
uniform mat4 view;
uniform mat4 cam_projection;

//projector info
uniform mat4 pjt_view;
uniform mat4 pjt_projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    // ProjTexCoord = pjt_projection * (model * vec4(FragPos, 1.0));
    // ProjTexCoord = pjt_projection * (vec4(FragPos, 1.0));
    
    // ProjTexCoord = cam_projection * view * vec4(FragPos, 1.0);
    ProjTexCoord = pjt_projection * pjt_view * vec4(FragPos, 1.0);

    
    gl_Position = cam_projection * view * vec4(FragPos, 1.0);
}