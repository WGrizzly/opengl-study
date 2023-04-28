#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//camera info
uniform mat4 model;
uniform mat4 camView;
uniform mat4 camProj;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = camProj * camView * vec4(FragPos, 1.0);
}