#version 330 core

struct Plane {
    vec3 norm;
    float d;
};

#define NR_PLANES 4

in vec3 frag_pos;
in vec3 normal;
in vec4 pjtTexCoord1;
in vec4 pjtTexCoord2;

uniform sampler2D pjtTexture;
float pjtFOV;    //in radian

uniform vec3 pjtPos1;
uniform vec3 pjtFront1;
uniform vec3 pjtPos2;
uniform vec3 pjtFront2;


uniform Plane pjtFrustumPlanes[NR_PLANES];

out vec4 FragColor;

const float EPSILON = 25e-04;
bool are_equal(float a, float b)
{
    return abs(a - b) < EPSILON;
}

bool pointIsOnPlane(Plane p, vec3 pos) {
    return abs(dot(p.norm, pos) + p.d) < EPSILON;
}

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

    if( 1.0 >= uv1.x && uv1.x >= 0. &&
        1.0 >= uv1.y && uv1.y >= 0. &&
        1.0 >= uv2.x && uv2.x >= 0. &&
        1.0 >= uv2.y && uv2.y >= 0.)
    {
        result = vec4(0.8);
    }

    // for(int c = 0; c < NR_PLANES; c++)
    // {
    //     if(pointIsOnPlane(pjtFrustumPlanes[c], frag_pos))
    //     {
    //         result = vec4(.0, .0, 1.0, 0.0);
    //         break;
    //     }
    // }

    // if(pointIsOnPlane(pjtFrustumPlanes[0], frag_pos))
    //     result = vec4(1.0, 1.0, 0.0, 0.0);
    // else if(pointIsOnPlane(pjtFrustumPlanes[1], frag_pos))
    //     result = vec4(1.0, 1.0, 0.0, 0.0);
    // else if(pointIsOnPlane(pjtFrustumPlanes[2], frag_pos))
    //     result = vec4(1.0, 1.0, 0.0, 0.0);
    // else if(pointIsOnPlane(pjtFrustumPlanes[3], frag_pos))
    //     result = vec4(1.0, 1.0, 0.0, 0.0);
    

    FragColor = result;
}