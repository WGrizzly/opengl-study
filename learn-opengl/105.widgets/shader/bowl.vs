#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 frag_pos;
out vec4 frag_pos4;
out vec3 normal;

//camera info
uniform mat4 model;
uniform mat4 camView;
uniform mat4 camProj;

//projector info
uniform mat4 pjtView1;
uniform mat4 pjtProjection1;
out vec4 pjtTexCoord1;

uniform mat4 pjtView2;
uniform mat4 pjtProjection2;
out vec4 pjtTexCoord2;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    frag_pos = FragPos;
    frag_pos4 = model * vec4(aPos, 1.0);
    
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;  

    pjtTexCoord1 = pjtProjection1 * pjtView1 * model * vec4(aPos, 1.0);
    pjtTexCoord2 = pjtProjection2 * pjtView2 * vec4(FragPos, 1.0);
    
    gl_Position = camProj * camView * vec4(FragPos, 1.0);
}