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

uniform samplerCube skybox;
uniform sampler2D   skysphere;

// Lights
uniform PointLight pLight;
uniform DirLight dirLight;
uniform int lMode;

struct MaterialProperties
{
    float roughness;
    float fresnel;
    vec3 diffCol;
    vec3 specCol;
    vec3 emissiveCol;
    float reflectivity;
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
    material.diffuse = materialProps[MatIdx].diffCol + Diffuse;
    material.specular = materialProps[MatIdx].specCol + vec3(Specular);
    material.emissive = materialProps[MatIdx].emissiveCol;
    material.roughness = materialProps[MatIdx].roughness;
    material.fresnel = materialProps[MatIdx].fresnel;
    material.reflectivity = materialProps[MatIdx].reflectivity;

    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Calculate reflection
    vec3 R = reflect(-viewDir, norm);
    vec4 reflectColor = texture(skybox, R) + texture(skysphere, vec2(R.x, R.y));
    material.diffuse = mix(material.diffuse, vec3(reflectColor.r, reflectColor.g, reflectColor.b), material.reflectivity);

    // Get the vertex view space position
    vec4 vVsPos = viewMat * vec4(FragPos, 1.0);

    // Calculate fragment shadow coefficient
    float shadow = CalcShadowCoef(uShadowMap, FragPos, vVsPos.xyz, uCascadesMatrices, uCascadesNear, uCascadesFar, uCascadesPlanes);

    // Calculate environment contribution (ambient)
    vec3 ambient = vec3(0.05);

    // Empty result
    vec3 result = vec3(0.0);

    if (lMode == 1)
        result += CalcDirLight(dirLight, norm, viewDir, material, shadow, ambient);
    else if (lMode == 2)
        result += CalcPointLight(pLight, norm, FragPos, viewDir, material, ambient);

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
