#version 330 core

in vec3 normal;
in vec4 pjtTexCoord1;
in vec4 pjtTexCoord2;

uniform sampler2D pjtTexture;
float pjtFOV;    //in radian

uniform vec3 pjtPos1;
uniform vec3 pjtFront1;
uniform vec3 pjtPos2;
uniform vec3 pjtFront2;

out vec4 FragColor;

void main()
{
    vec4 result = vec4(0.8);
    // result += vec4(textureProj(pjtTexture, pjtTexCoord).rgb, 1.0);
    // result += textureProj(pjtTexture, pjtTexCoord);

    // if(pjtTexCoord1.z > 0.)
    // {
    //     vec2 uv = pjtTexCoord1.xy / pjtTexCoord1.z;
    //     if( 1.0 >= uv.x && uv.x >= 0. &&
    //         1.0 >= uv.y && uv.y >= 0.)
    //     {
    //         result += texture(pjtTexture, uv);
    //     }
    //     else
    //         result = vec4(0.6);
    // }

    vec2 uv1 = pjtTexCoord1.xy / pjtTexCoord1.z;
    if( 1.0 >= uv1.x && uv1.x >= 0. &&
        1.0 >= uv1.y && uv1.y >= 0.)
    {
        result += texture(pjtTexture, uv1);
    }
    vec2 uv2 = pjtTexCoord2.xy / pjtTexCoord2.z;
    if( 1.0 >= uv2.x && uv2.x >= 0. &&
        1.0 >= uv2.y && uv2.y >= 0.)
    {
        result += texture(pjtTexture, uv2);
    }

    FragColor = result;
}