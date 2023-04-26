#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 FragNorm;
in vec2 TexCoords;
in vec4 ProjTexCoord;
in mat4 PjtProjMat;

uniform sampler2D texture_diffuse1;
uniform sampler2D pjt_tex;
uniform vec3 pjt_pos;

void main()
{    
    vec4 objTexColor = texture(texture_diffuse1, TexCoords);

    // vec4 projTexColor = vec4(textureProj(pjt_tex, ProjTexCoord).rgb, 1.0);
    vec4 texCoords = PjtProjMat * vec4(FragPos, 1.0);
    texCoords.xy /= texCoords.w;
    texCoords.xy = texCoords.xy * 0.5 + 0.5;
    vec4 projTexColor = texture(pjt_tex, texCoords.xy);

    FragColor = projTexColor;

    // vec3 vec_prj_dir = normalize(FragPos - pjt_pos);
    // if( (FragPos.x >= 0.0 && 1.0 >= FragPos.x) )
    // {
    //     FragColor = vec4(0.f, 0.8f, 0.8f, 0.f);
    // }
    // else
    // {
    //     FragColor = vec4(0.8f, 0.8f, 0.0f, 0.f);
    // }


    // FragColor = texture(prj_tex, TexCoords);
    // FragColor = vec4(0.f, 0.8f, 0.8f, 0.f);
    // FragColor = vec4(textureProj(pjt_tex, ProjTexCoord).rgb, 1.0);

    // vec3 norm_test = normalize(FragPos - pjt_pos);
    // if(FragNorm.x > 0)
    // // if(norm_test.x > 0)
    //     FragColor = vec4(0.f, 0.8f, 0.8f, 0.f);
    // else
    //     FragColor = vec4(0.8f, 0.8f, 0.0f, 0.f);

    // // FragColor = vec4(textureProj(pjt_tex, ProjTexCoord).rgb, 1.0);
}