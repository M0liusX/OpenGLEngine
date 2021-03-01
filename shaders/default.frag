#version 460 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D s;

void main()
{
    outColor = texture(s, Texcoord) * vec4(Color, 1.0);
}