#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uvCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 UVCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)
{
    UVCoords = uvCoords;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalMatrix * normal;

    vec4 worldPos = model * vec4(position, 1.0f);
    FragPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}
