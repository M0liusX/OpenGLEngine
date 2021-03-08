#version 460 core

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

in vec3 position;
in vec3 normal;
in vec3 offset;
in vec3 color;
// in vec2 texcoord;

out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
// out vec2 Texcoord;

void main()
{
    Color = color;
    Normal = normal;
    // Texcoord = texcoord;
    gl_Position = MVP * vec4(position + offset, 1.0);
    FragPos = vec3(position + offset);
}