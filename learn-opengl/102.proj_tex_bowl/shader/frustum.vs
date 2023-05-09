#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 inv_proj;
uniform mat4 view;

void main()
{
   gl_Position = inv_proj * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}