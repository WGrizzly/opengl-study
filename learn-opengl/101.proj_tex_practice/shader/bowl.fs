#version 330 core

in vec3 normal;
in vec4 pjtTexCoord;

uniform sampler2D pjtTexture;
uniform vec3 pjtPos;
uniform vec3 pjtFront;
float pjtFOV;    //in radian

out vec4 FragColor;

void main()
{
    vec4 result = vec4(0.8);
    // result += vec4(textureProj(pjtTexture, pjtTexCoord).rgb, 1.0);
    // result += textureProj(pjtTexture, pjtTexCoord);

    vec2 uv = pjtTexCoord.xy / pjtTexCoord.z;
    result += texture(pjtTexture, uv);
    FragColor = result;
}