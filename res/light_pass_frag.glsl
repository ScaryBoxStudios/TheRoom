#version 330
#include lighting
#include shadowing
out vec4 color;

// GBuffer data
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform usampler2D gMatIdx;
uniform ivec2 gScreenSize;
uniform vec3 viewPos;

// Shadows
uniform sampler2DArray uShadowMap;
uniform float uCascadesNear[4];
uniform float uCascadesFar[4];
uniform vec2 uCascadesPlanes[4];
uniform mat4 uCascadesMatrices[4];
uniform mat4 viewMat;

// Lights
uniform PointLight pLight;
uniform DirLight dirLight;
uniform int lMode;

struct MaterialProperties
{
    float roughness;
    float fresnel;
    float metallic;
    float transparency;
    vec3  diffCol;
    vec3  specCol;
    vec3  emissiveCol;
};

// UBO holding the material data
layout (std140) uniform MaterialDataBlock
{
    MaterialProperties materialProps[64];
};

// --------------------------------------------------
// Main section
// --------------------------------------------------
void main(void)
{
    // Retrieve data from GBuffer
    vec2 UVCoords = gl_FragCoord.xy / gScreenSize;
    vec3 FragPos = texture(gPosition, UVCoords).rgb;
    vec3 Normal = texture(gNormal, UVCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, UVCoords).rgb;
    float Specular = texture(gAlbedoSpec, UVCoords).a;
    uint MatIdx = texture(gMatIdx, UVCoords).r;

    // Fill material struct
    Material material;
    material.diffuse   = materialProps[MatIdx].diffCol + Diffuse;
    material.specular  = materialProps[MatIdx].specCol + vec3(Specular);
    material.emissive  = materialProps[MatIdx].emissiveCol;
    material.roughness = materialProps[MatIdx].roughness;
    material.fresnel   = materialProps[MatIdx].fresnel;
    material.metallic  = materialProps[MatIdx].metallic;
    material.transparency = materialProps[MatIdx].transparency;

    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Get the vertex view space position
    vec4 vVsPos = viewMat * vec4(FragPos, 1.0);

    // Calculate fragment shadow coefficient
    float shadow = CalcShadowCoef(uShadowMap, FragPos, vVsPos.xyz, uCascadesMatrices, uCascadesNear, uCascadesFar, uCascadesPlanes);

    // Empty result
    vec3 result = vec3(0.0);

    if (lMode == 1)
        result += CalcDirLight(dirLight, norm, viewDir, material, shadow);
    else if (lMode == 2)
        result += CalcPointLight(pLight, norm, FragPos, viewDir, material);
    else if (lMode == 3)
        result += CalcEnvLight(norm, FragPos, viewDir, material);

    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir, material);

    color = vec4(result, 1.0);

    // Debug shadow splits
    //if (lMode == 1)
    //{
    //    vec4 cascadeWeights = GetCascadeWeights(
    //        -vVsPos.z,
    //        vec4(uCascadesPlanes[0].x, uCascadesPlanes[1].x, uCascadesPlanes[2].x, uCascadesPlanes[3].x),
    //        vec4(uCascadesPlanes[0].y, uCascadesPlanes[1].y, uCascadesPlanes[2].y, uCascadesPlanes[3].y)
    //    );
    //    color.rgb = color.rgb * GetCascadeColor(cascadeWeights);
    //}
}
