#version 460 core

// Camera Projection
uniform mat4 MVP;

// Mesh Properties
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

// Transformation
layout (location = 3) in vec3 offset;
layout (location = 4) in vec3 rotation;
layout (location = 5) in vec3 scale;

// Fragment Input
out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
out vec2 Texcoord;

void main()
{
    Color = vec3(1.0, 1.0, 1.0);
    Normal = normal;
    Texcoord = texcoord;
    vec3 scaled_position = position * scale;
    gl_Position = MVP * vec4(scaled_position + offset , 1.0);
    FragPos = vec3(scaled_position + offset);
}