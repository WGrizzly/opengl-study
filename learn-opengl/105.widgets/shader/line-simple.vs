#version 330 core

layout (location = 0) in vec4 world_pos;

uniform mat4 cam_proj;
uniform mat4 cam_view;

void main()
{
    gl_Position = cam_proj * cam_view * world_pos;
}