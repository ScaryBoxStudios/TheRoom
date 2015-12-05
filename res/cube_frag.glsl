#version 330

in vec3 Normal;
in vec3 Color;
in vec2 UVCoords;

out vec4 color;

uniform sampler2D tex;

void main(void)
{
    float ambientStrength = 0.3f;
    vec3 lightColor = vec3(1.0f);
    vec4 ambient = vec4(ambientStrength * lightColor, 1);

    color = texture(tex, UVCoords) * vec4(Color, 1) * ambient;
}
