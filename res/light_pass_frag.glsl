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
// Shadows section
// --------------------------------------------------
vec4 GetCascadeWeights(float depth, vec4 splitNears, vec4 splitFars)
{
    vec4 near = step(splitNears, vec4(depth));
    vec4 far = step(depth, splitFars);
    return near * far;
}

float GetCascadeLayer(vec4 weights)
{
    return 0.0 * weights.x
         + 1.0 * weights.y
         + 2.0 * weights.z
         + 3.0 * weights.w;
}

mat4 GetCascadeViewProjection(vec4 weights, mat4 viewProj[4])
{
    return viewProj[0] * weights.x
         + viewProj[1] * weights.y
         + viewProj[2] * weights.z
         + viewProj[3] * weights.w;
}

float GetCascadeZ(vec4 weights, float z[4])
{
    return z[0] * weights.x
         + z[1] * weights.y
         + z[2] * weights.z
         + z[3] * weights.w;
}

float GetCascadeNear(vec4 weights, float zNears[4])
{
    return zNears[0] * weights.x
         + zNears[1] * weights.y
         + zNears[2] * weights.z
         + zNears[3] * weights.w;
}

float GetCascadeFar(vec4 weights, float zFars[4])
{
    return zFars[0] * weights.x
         + zFars[1] * weights.y
         + zFars[2] * weights.z
         + zFars[3] * weights.w;
}

vec3 GetCascadeColor(vec4 weights)
{
    return vec3(1,0,0) * weights.x
         + vec3(0,1,0) * weights.y
         + vec3(0,0,1) * weights.z
         + vec3(1,0,1) * weights.w;
}

bool IsVertexInShadowMap(vec3 coord)
{
    return coord.z > 0.0
        && coord.x > 0.0
        && coord.y > 0.0
        && coord.x <= 1.0
        && coord.y <= 1.0;
}

float CalcShadowCoef(
    sampler2DArray shadowMap,
    vec3 vWSPos,
    vec3 vVSPos,
    mat4 viewProjs[4],
    float zNears[4],
    float zFars[4],
    vec2 splitPlanes[4]
)
{
    // Find frustum section
    vec4 cascadeWeights = GetCascadeWeights(
        -vVSPos.z,
        vec4(splitPlanes[0].x, splitPlanes[1].x, splitPlanes[2].x, splitPlanes[3].x),
        vec4(splitPlanes[0].y, splitPlanes[1].y, splitPlanes[2].y, splitPlanes[3].y)
    );

    // Get vertex position in light space
    mat4 viewProj = GetCascadeViewProjection(cascadeWeights, viewProjs);
    vec4 vertexLightPos = viewProj * vec4(vWSPos, 1.0);
    // Perform perspective divide
    vec3 coords = vertexLightPos.xyz / vertexLightPos.w;
    // Transform to [0,1] range
    coords = coords * 0.5 + 0.5;

    if(!IsVertexInShadowMap(coords))
        return 0.0;

    // Fetch near, far values and the layer index of the current used cascade
    float near = GetCascadeNear(cascadeWeights, zNears);
    float far = GetCascadeFar(cascadeWeights, zFars);
    float layer = GetCascadeLayer(cascadeWeights);
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, vec3(coords.xy, layer)).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = coords.z;

    // Calculate shadow term
    float bias = 0.0052;
    // No PCF
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(coords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // Return shadow factor
    return shadow;
}

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

    // Empty result
    vec3 result = vec3(0.0);

    if (lMode == 1)
        result += CalcDirLight(dirLight, norm, viewDir, material, shadow);
    else if (lMode == 2)
        result += CalcPointLight(pLight, norm, FragPos, viewDir, material);

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
