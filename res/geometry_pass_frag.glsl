#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 UVCoords;

struct Material
{
    vec3 diffuseColor;
    sampler2D diffuseTexture;
    float specularColor;
    sampler2D specularTexture;
};

uniform Material material;

void main(void)
{
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = material.diffuseColor + texture(material.diffuseTexture, UVCoords).rgb;
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = material.specularColor + texture(material.specularTexture, UVCoords).r;
}

