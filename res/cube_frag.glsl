#version 330

in vec3 vertexColor;
in vec2 texCoords;

out vec4 color;

uniform sampler2D tex;

void main(void)
{
    float ambientStrength = 0.3f;
    vec3 lightColor = vec3(1.0f);
    vec4 ambient = vec4(ambientStrength * lightColor, 1);

    color = texture(tex, texCoords) * vec4(vertexColor, 1) * ambient;
}
