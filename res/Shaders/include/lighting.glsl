#module lighting
#include material
#include brdf
#include math

#extension GL_ARB_texture_query_levels : enable
#extension GL_ARB_gpu_shader5 : enable

uniform samplerCube skybox;
uniform samplerCube irrMap;
uniform samplerCube radMap;

// Attenuation light properties
struct AttenuationProps
{
    float constant;
    float linear;
    float quadratic;
};

// Directional light
struct DirLight
{
    vec3 color;
    vec3 direction;
};

// Point light
struct PointLight
{
    vec3 color;
    AttenuationProps attProps;
    vec3 position;
};

// Spot light
struct SpotLight
{
    vec3 color;
    AttenuationProps attProps;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
};

// Calculates light attenuation value, according to its attenuation properties, the light position, and the fragment position
float CalcAttenuationValue(AttenuationProps attProps, vec3 lightPos, vec3 fragPos)
{
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (attProps.constant + attProps.linear * distance + attProps.quadratic * (distance * distance));
    return attenuation;
}

vec3 CalcLight(vec3 lightColor, vec3 normal, vec3 lightDir, vec3 viewDir, Material material, float shadowFactor)
{
    // Helper variables
    vec3  baseColor    = material.diffuse;
    vec3  emissive     = material.emissive;
    float roughness    = material.roughness;
    float reflectivity = material.fresnel;
    float metallic     = material.metallic;
    float transparency = material.transparency;
    float NdotL = saturate(dot(normal, lightDir));

    // Calculate visibility
    float visibility = 1.0 - shadowFactor;

    // BRDF
    vec3 brdfResult = BRDF(normal, lightDir, viewDir, baseColor, metallic, roughness, reflectivity);

    // Combine results
    return lightColor * NdotL * (brdfResult + emissive) * visibility;
}

// Calculates the enviroment light contribution
vec3 CalcEnvLight(vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 baseColor     = material.diffuse;
    float roughness    = material.roughness;
    float reflectivity = material.fresnel;
    float metallic     = material.metallic;

    // Calculate reflection dir
    vec3 reflectDir = reflect(-viewDir, normal);

    // Calculate mipmap level based on roughness
    float mipmapLevel = min(roughness * 10, 6);

    // Get radiance and irradiance colors
    vec3 rad = textureLod(radMap, reflectDir, mipmapLevel).rgb;
    //vec3 rad = texture(skybox, reflectDir).rgb;
    vec3 irr = texture(irrMap, reflectDir).rgb;
    irr *= material.diffuse;

    // BRDF
    vec3 brdfResult = BRDF(normal, reflectDir, viewDir, baseColor, metallic, roughness, reflectivity, irr, rad);
    return brdfResult;
}

// Calculates the color of directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    // Color
    vec3 color = CalcLight(2 * light.color, normal, lightDir, viewDir, material, shadow);
    return color;
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Color
    vec3 color = CalcLight(light.color, normal, lightDir, viewDir, material, 0.0);
    // Attenuation
    float attenuation = CalcAttenuationValue(light.attProps, light.position, fragPos);
    return color * attenuation;
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Color
    vec3 color = CalcLight(light.color, normal, lightDir, viewDir, material, 0.0);
    // Attenuation
    float attenuation = CalcAttenuationValue(light.attProps, light.position, fragPos);
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    return color * attenuation * intensity;
}

//--------------------------------------------------
// Legacy
//--------------------------------------------------
// Calculates specular intensity according to the Phong model
float CalcPhongSpec(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess)
{
    vec3 reflectDir = reflect(-lightDir, normal);
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float spec = pow(specAngle, shininess);
    return spec;
}

// Calculates specular intensity according to the Blinn - Phong model
float CalcBlinnPhongSpec(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess)
{
    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    float spec = pow(specAngle, shininess * 4);
    return spec;
}
