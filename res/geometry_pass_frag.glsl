#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 UVCoords;
in mat3 TBN;

struct Material
{
    vec3 diffuseColor;
    sampler2D diffuseTexture;
    float specularColor;
    sampler2D specularTexture;
};

uniform Material material;

uniform sampler2D normalMap;
uniform bool useNormalMaps;

void main(void)
{
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;

    // Also store the per-fragment normals into the gbuffer
    if(useNormalMaps)
    {
        gNormal = texture(normalMap, UVCoords).rgb;
        gNormal = normalize(gNormal * 2.0 - 1.0);
        gNormal = normalize(TBN * gNormal);
    }
    else
    {
        gNormal = normalize(Normal);
    }

    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = material.diffuseColor + texture(material.diffuseTexture, UVCoords).rgb;

    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = material.specularColor + texture(material.specularTexture, UVCoords).r;
}

