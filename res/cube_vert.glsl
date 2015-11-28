#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;
layout(location=2) in vec2 texUV;

out vec3 vertexColor;
out vec2 texCoords;

uniform mat4 MVP;

void main(void)
{
    texCoords = texUV;
    vertexColor = color;
    gl_Position = MVP * vec4(position, 1.0f);
}
