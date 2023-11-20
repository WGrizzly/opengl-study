#version 330 core

// layout (location = 0) in vec3 aPos;
layout (location = 0) in vec4 aPos;

uniform mat4 cam_proj;
uniform mat4 cam_view;

uniform mat4 pjt_proj;
uniform mat4 pjt_view;

uniform mat4 hm;

void main()
{
    vec4 ndc_pt = aPos;
    mat4 inv = inverse(pjt_proj * pjt_view);
    // mat4 inv = inverse(cam_proj * cam_view);

    vec4 world_pos = inv * ndc_pt;

    vec4 transformed_pos = hm * world_pos;

    // gl_Position = cam_proj * cam_view * transformed_pos;
    gl_Position = cam_proj * cam_view * world_pos;
    // gl_Position = cam_proj * cam_view * aPos;
}