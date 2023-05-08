#version 330 core

in vec3 normal;
in vec4 pjtTexCoord1;

uniform sampler2D pjtTexture;
uniform vec3 pjtPos1;
uniform vec3 pjtFront1;
float pjtFOV;    //in radian

out vec4 FragColor;

void main()
{
    vec4 result = vec4(0.8);
    // result += vec4(textureProj(pjtTexture, pjtTexCoord).rgb, 1.0);
    // result += textureProj(pjtTexture, pjtTexCoord);

    vec2 uv = pjtTexCoord1.xy / pjtTexCoord1.z;
    result += texture(pjtTexture, uv);
    FragColor = result;
}