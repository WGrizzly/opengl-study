//exercise 6.3
#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    //1. since 'z' pos fixed to zero, blue color is not available.
    //2. crds system is using normalized, left-bottom is negative.
    //3. when vertex is located at left-bottom, r,g has neg value, and b is fixed to zero.
    //   that's the reason why color is black.
    vec3 new_color = ourColor;
    // new_color.z = 1.f;
    new_color.z = 1.f - (ourColor.x + ourColor.y);
    FragColor = vec4(new_color, 1.0f);
}

// #version 330 core
// out vec4 FragColor;
//
// in vec3 ourColor;
//
// void main()
// {
//     FragColor = vec4(ourColor, 1.0f);
// }