#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 cam_projection;

void main()
{
	gl_Position = cam_projection * view * model * vec4(aPos, 1.0);
}