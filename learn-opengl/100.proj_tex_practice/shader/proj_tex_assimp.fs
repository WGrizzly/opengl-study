#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec4 ProjTexCoord;

uniform sampler2D pjt_tex;
uniform vec3 pjt_pos;

void main()
{    
    // FragColor = texture(prj_tex, TexCoords);
    // FragColor = vec4(0.f, 0.8f, 0.8f, 0.f);
    // FragColor = vec4(textureProj(pjt_tex, ProjTexCoord).rgb, 1.0);

    vec3 norm_test = normalize(FragPos - pjt_pos);
    if(norm_test.x > 0)
        FragColor = vec4(0.f, 0.8f, 0.8f, 0.f);
    else
        FragColor = vec4(0.8f, 0.8f, 0.0f, 0.f);

}