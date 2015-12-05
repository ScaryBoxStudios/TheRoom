#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in vec2 uvCoords;

out vec3 Color;
out vec2 UVCoords;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)
{
    UVCoords = uvCoords;
    Color = color;
    Normal = normal;
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
