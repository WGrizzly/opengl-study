#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    // float ambientStrength = 0.1;
    // vec3 ambient = lightColor * ambientStrength;

    // vec3 result = ambient * objectColor;
    // FragColor = vec4(result, 1.0);
    FragColor = vec4(lightColor * objectColor, 1.0);
}