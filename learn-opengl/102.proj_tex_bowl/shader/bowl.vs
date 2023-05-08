#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 normal;

//camera info
uniform mat4 model;
uniform mat4 camView;
uniform mat4 camProj;

//projector info
uniform mat4 pjtView1;
uniform mat4 pjtProjection1;
out vec4 pjtTexCoord1;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;  

    pjtTexCoord1 = pjtProjection1 * pjtView1 * vec4(FragPos, 1.0);
    
    gl_Position = camProj * camView * vec4(FragPos, 1.0);
}