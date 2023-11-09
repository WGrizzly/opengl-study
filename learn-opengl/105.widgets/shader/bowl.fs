#version 330 core

struct Plane {
    vec3 norm;
    float d;
};

struct Line{
    vec3 point;
    vec3 direction;
};

#define NR_PLANES 8
#define BLEND_WIDTH 0.15
#define PI 3.1415926535897932384626433832795

in vec3 frag_pos;
in vec4 frag_pos4;
in vec3 normal;
in vec4 pjtTexCoord1;
in vec4 pjtTexCoord2;

uniform sampler2D pjtTexture0;
uniform sampler2D pjtTexture1;
float pjtFOV;    //in radian

uniform vec3 pjtPos1;
uniform vec3 pjtFront1;
uniform vec3 pjtPos2;
uniform vec3 pjtFront2;


uniform Plane pjtFrustumPlanes[NR_PLANES];
uniform Plane pjtBlendPlane;
uniform Line up_line;
uniform Line down_line;

out vec4 FragColor;

const float EPSILON = 25e-04;
bool are_equal(float a, float b)
{
    return abs(a - b) < EPSILON;
}

bool pointIsOnPlane(Plane p, vec3 pos) {
    return abs(dot(p.norm, pos) + p.d) < EPSILON;
}

float getDistance(vec3 point, vec3 linePoint, vec3 lineDirection)
{
    vec3 diff = point - linePoint;
    vec3 crossProduct = cross(diff, lineDirection);
    float distance = length(crossProduct) / length(lineDirection);
    return distance;
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
        // result += texture(pjtTexture, uv1);
        // result += textureProj(pjtTexture, pjtTexCoord1);

        result = textureProj(pjtTexture0, pjtTexCoord1);
    }
    vec2 uv2 = pjtTexCoord2.xy / pjtTexCoord2.z;
    if( 1.0 >= uv2.x && uv2.x >= 0. &&
        1.0 >= uv2.y && uv2.y >= 0.)
    {
        // result += texture(pjtTexture, uv2);
        result = texture(pjtTexture1, uv2);
    }

    //case of overlapped region
    if( 1.0 >= uv1.x && uv1.x >= 0. &&
        1.0 >= uv1.y && uv1.y >= 0. &&
        1.0 >= uv2.x && uv2.x >= 0. &&
        1.0 >= uv2.y && uv2.y >= 0.)
    {
        float numerator = dot(pjtBlendPlane.norm, frag_pos) + pjtBlendPlane.d;
        float denominator = length(pjtBlendPlane.norm);
        float dist = numerator / denominator;
        float dist_abs = abs(dist);
        float dist_blend = dist_abs / BLEND_WIDTH;

        vec4 tex1_intensity = textureProj(pjtTexture0, pjtTexCoord1);
        vec4 tex2_intensity = 1.0 - textureProj(pjtTexture1, pjtTexCoord2);

        if(true)
        {
            //linear blending
            float w = 0.f;
            if( 1.f > dist_blend)
            {
                w = 1.f - dist_blend;
                w /= 2.f;
            }

            if( 0.f > dist )
                result = tex1_intensity * (1.f - w) + tex2_intensity * w;
            else
                result = tex2_intensity * (1.f - w) + tex1_intensity * w;
        }
        else
        {
            //gaussian blending
            float w = 0.f;
            if( 1.f > dist_blend)
            {
                float sigma = 0.27;
                float sigma_sqr = sigma * sigma;
                float sigma_max_val = 1 / (sqrt(2 * PI)*sigma); 
                float gval = sigma_max_val * exp( -dist_blend*dist_blend / (2 * sigma_sqr));
                gval /= sigma_max_val;
                gval *= 0.5f;

                w = gval;
            }

            if( 0.f > dist )
                result = tex1_intensity * (1.f - w) + tex2_intensity * w;
            else
                result = tex2_intensity * (1.f - w) + tex1_intensity * w;
        }
    }


    FragColor = result;
}