#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
  
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D texture1;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = lightColor * ambientStrength;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (specular + diffuse + ambient) * objectColor;
    // FragColor = vec4(result , 1.0);

    vec4 tex_val = texture(texture1, TexCoord);
    FragColor = (vec4(result , 1.0) + tex_val);
}