#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 cam_proj;
uniform mat4 cam_view;

uniform mat4 pjt_proj;
uniform mat4 pjt_view;

void main()
{
    vec4 ndc_pt = vec4(aPos, 1.0);
    mat4 inv = inverse(pjt_proj * pjt_view);
    vec4 world_pos = inv * ndc_pt;

    gl_Position = cam_proj * cam_view * world_pos;
}