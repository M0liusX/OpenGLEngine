#version 460 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
// in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D s;

void main()
{
    vec3 lightpos =   vec3(0.0, 0.0, 1.0);
    vec3 lightcolor = vec3(1.0, 1.0, 1.0);
    float ambient = 0.1f;
    //outColor = texture(s, Texcoord) * vec4(Color, 1.0);
    vec3 norm = normalize(Normal);
    vec3 lightdir = normalize(lightpos - FragPos);
    float diff = max(dot(norm, lightdir), 0.0) * 0.8;
    vec3 diffuse = diff * lightcolor;
    vec3 result = (ambient + diffuse) * Color;
    outColor = vec4(result, 1.0);
}