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
uniform vec3 pjtFrustPts[8];
uniform mat4 pjtProjOrg;

out vec4 FragColor;

uniform mat4 pjtView1;
uniform mat4 pjtProjection1;

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


    // vec4 a_ndc = vec4(1.,  1.,  1., 1.);
    // vec4 b_ndc = vec4(1.,  1., -1., 1.);
    // vec4 c_ndc = vec4(1., -1., -1., 1.);
    //
    // mat4 im = inverse(pjtProjOrg * pjtView1);
    // vec4 a_world = im * a_ndc;
    // vec4 b_world = im * b_ndc;
    // vec4 c_world = im * c_ndc;
    //
    // vec3 ab = vec3(b_world - a_world);
    // vec3 ac = vec3(c_world - a_world);
    // vec3 n = normalize(cross(ab, ac));
    // float d = -dot(n, vec3(a_world));
    //
    // float check_val = abs( frag_pos.x*n.x + frag_pos.y*n.y + frag_pos.z*n.z + d );
    // if(check_val < EPSILON)
    //     result = vec4(0., 0., 1.0, 0.0);
    //
    // if(EPSILON > abs(1.f - frag_pos.x))
    //     result = vec4(0., 1., .0, 0.0);
    // if(EPSILON > abs(0.f - frag_pos.x))
    //     result = vec4(1., 0., .0, 0.0);
    

    FragColor = result;
}