#version 330
#include lighting
out vec4 color;

// GBuffer data
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform usampler2D gMatIdx;
uniform ivec2 gScreenSize;
uniform vec3 viewPos;

// Shadows
uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;
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

void main(void)
{
    // Retrieve data from GBuffer
    vec2 UVCoords = gl_FragCoord.xy / gScreenSize;
    vec3 FragPos = texture(gPosition, UVCoords).rgb;
    vec3 Normal = texture(gNormal, UVCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, UVCoords).rgb;
    float Specular = texture(gAlbedoSpec, UVCoords).a;
    uint MatIdx = texture(gMatIdx, UVCoords).r;

    // Calculate the fragment light space position
    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

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

    // Calculate fragment shadow
    float shadow = ShadowCalculation(FragPosLightSpace, norm, -dirLight.direction, FragPos, shadowMap);

    // Empty result
    vec3 result = vec3(0.0);

    if (lMode == 1)
        result += CalcDirLight(dirLight, norm, viewDir, material, shadow);
    else if (lMode == 2)
        result += CalcPointLight(pLight, norm, FragPos, viewDir, material);

    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir, material);

    color = vec4(result, 1.0);
    //color = vec4(texture(shadowMap, UVCoords).r);
}
