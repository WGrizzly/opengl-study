#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 cam_proj;
uniform mat4 cam_view;

uniform mat4 pjt_proj;
uniform mat4 pjt_view;

void main()
{
    vec4 ndc_pt = vec4(aPos, 1.0);
    mat4 inv = inverse(cam_proj * cam_view);
    vec4 world_pos = inv * ndc_pt;

    gl_Position = cam_proj * cam_view * world_pos;

    // vec4 FragPos = vec4(aPos, 1.0);
    // gl_Position = cam_proj * cam_view * FragPos;

   //  gl_Position = inverse(cam_view * cam_view) * FragPos;
   // gl_Position = FragPos;
 
    // vec4 world_pos = inverse(pjt_view) * vec4(aPos, 1.0);
    // vec4 view_pos = cam_view * world_pos;
    // gl_Position = cam_proj * view_pos;
 
    // vec4 screen_pos = vec4(aPos, 1.0);
    // mat4 m = inverse(pjt_proj * pjt_view);
    // gl_Position = m * screen_pos;

   //  vec4 world_pos = inverse(pjt_proj * pjt_view) * vec4(aPos, 1.0);
   //  vec3 vert = vec3(world_pos / world_pos.w);
   //  gl_Position = vec4(vert, 1.0f);
}