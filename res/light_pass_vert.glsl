#version 330

layout(location=0) in vec3 position;
layout(location=2) in vec2 uvCoords;

out vec2 UVCoords;

void main(void)
{
    UVCoords = uvCoords;
    gl_Position = vec4(position, 1.0f);
}
