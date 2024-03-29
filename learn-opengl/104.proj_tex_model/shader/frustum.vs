#version 330 core

// layout (location = 0) in vec3 aPos;
layout (location = 0) in vec4 aPos;

uniform mat4 cam_proj;
uniform mat4 cam_view;

uniform mat4 pjt_proj;
uniform mat4 pjt_view;

bool direct_test;

void main()
{
    // vec4 ndc_pt = vec4(aPos, 1.0);
    vec4 ndc_pt = aPos;
    mat4 inv = inverse(pjt_proj * pjt_view);

    vec4 world_pos = inv * ndc_pt;

    gl_Position = cam_proj * cam_view * world_pos;
    // gl_Position = cam_proj * cam_view * aPos;
}