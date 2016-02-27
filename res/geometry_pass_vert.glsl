#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoords;
layout (location = 3) in vec3 tangent;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 UVCoords;
    mat3 TBN;
} vsOut;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)
{
    vsOut.UVCoords = uvCoords;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vsOut.Normal = normalMatrix * normal;

    // Calcullate TBN
    vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(normal,    0.0)));

    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);

    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(T, N);

    // Generate TBN matrix
    vsOut.TBN = mat3(T, B, N);

    vec4 worldPos = model * vec4(position, 1.0f);
    vsOut.FragPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}
