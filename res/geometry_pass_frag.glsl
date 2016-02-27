#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out uint gMatIdx;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 UVCoords;
    mat3 TBN;
} fsIn;

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
uniform uint matIdx;

void main(void)
{
    // Store the fragment position vector in the first gbuffer texture
    gPosition = fsIn.FragPos;

    // Also store the per-fragment normals into the gbuffer
    if(useNormalMaps)
    {
        gNormal = texture(normalMap, fsIn.UVCoords).rgb;
        gNormal = normalize(gNormal * 2.0 - 1.0);
        gNormal = normalize(fsIn.TBN * gNormal);
    }
    else
    {
        gNormal = normalize(fsIn.Normal);
    }

    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = material.diffuseColor + texture(material.diffuseTexture, fsIn.UVCoords).rgb;

    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = material.specularColor + texture(material.specularTexture, fsIn.UVCoords).r;

    // Store the material index
    gMatIdx = matIdx;
}

