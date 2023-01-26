#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;

uniform vec2 hoffset;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos.x + hoffset.x, aPos.y * -1.f + hoffset.y, aPos.z, 1.0);
    ourColor = gl_Position.xyz;
}

////exercise 6.2
// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
//
// uniform vec2 hoffset;
//
// out vec3 ourColor;
//
// void main()
// {
//     gl_Position = vec4(aPos.x + hoffset.x, aPos.y * -1.f + hoffset.y, aPos.z, 1.0);
//     ourColor = aColor;
// }

// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
//
// out vec3 ourColor;
//
// void main()
// {
//     gl_Position = vec4(aPos, 1.0);
//     ourColor = aColor;
// }