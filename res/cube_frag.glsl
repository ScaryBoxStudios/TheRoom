#version 330

in vec3 vertexColor;
in vec2 texCoords;

out vec4 color;

uniform sampler2D tex;

void main(void)
{
    color = texture(tex, texCoords) * vec4(vertexColor, 1);
}
