#version 460 core

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

in vec3 position;
in vec3 color;
in vec2 texcoord;

out vec3 Color;
out vec2 Texcoord;

void main()
{
    Color = color;
    Texcoord = texcoord;
    gl_Position = MVP * vec4(position, 1.0);
}