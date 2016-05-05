#module lighting
#extension GL_ARB_texture_query_levels : enable

uniform samplerCube skybox;
uniform sampler2D   skysphere;

float saturate(float v) { return clamp(v, 0.0, 1.0); }
vec3 saturate(vec3 v) { return clamp(v, 0.0, 1.0); }

struct Material
{
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float roughness;
    float fresnel;
    float metallic;
};

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

float pi = 3.14159265;

// Calculates light attenuation value, according to its attenuation properties, the light position, and the fragment position
float CalcAttenuationValue(AttenuationProps attProps, vec3 lightPos, vec3 fragPos)
{
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (attProps.constant + attProps.linear * distance + attProps.quadratic * (distance * distance));
    return attenuation;
}

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

float GeometricalAttenuation(float NdotH, float NdotV, float VdotH, float NdotL)
{
    float NH2 = 2.0 * NdotH;
    float g1  = (NH2 * NdotV) / VdotH;
    float g2  = (NH2 * NdotL) / VdotH;
    return min(1.0, min(g1, g2));
}

float BeckmannDistribution(float roughness, float NdotH)
{
    float a  = roughness * roughness;
    float NdotH2 = NdotH * NdotH;
    return exp((NdotH2 - 1.0) / (a * NdotH2)) / (pi * a * pow(NdotH, 4.0));
}

// Slick's approximation
// Pass the reflected color at normal angle directly
vec3 Fresnel(vec3 R0, float VdotH)
{
    float u5 = pow(1.0 - VdotH, 5);
    return R0.rgb + (vec3(1.0) - R0.rgb) * vec3(u5);
}

vec3 BRDF(vec3 N, vec3 L, vec3 V, vec3 baseColor, float metallic, float roughness, float reflectivity)
{
    vec3  H     = normalize(L + V);
    float NdotL = saturate(dot(N, L));
    float NdotH = max(1e-5, dot(N, H));
    float NdotV = max(1e-5, dot(N, V));
    float VdotH = clamp(dot(V, H), 1e-5, 1.0);

    // Specular vs Diffuse
    vec3 C0 = baseColor * metallic + vec3(1.0) * (1.0 - metallic);

    // Calculate fresnel
    vec3 F = Fresnel(C0, VdotH);

    // Specular and diffuse contributions
    vec3 Ks = F * reflectivity;
    vec3 Kd = (1.0 - Ks) * (1.0 - metallic); // * (1.0 - transparency);

    // Calculate Distribution and Geometrical Attenuation
    float G = GeometricalAttenuation(NdotH, NdotV, VdotH, NdotL);
    float D = BeckmannDistribution(roughness, NdotH);

    // Final results
    vec3 specular = Ks * D * G / (NdotV * NdotL * 4);
    vec3 diffuse  = Kd * baseColor / pi;
    return specular + diffuse;
}

vec3 CalcLight(vec3 lightColor, vec3 normal, vec3 lightDir, vec3 viewDir, Material material, float shadowFactor)
{
    // Helper variables
    vec3  baseColor    = material.diffuse;
    vec3  emissive     = material.emissive;
    float roughness    = material.roughness;
    float reflectivity = material.fresnel;
    float metallic     = material.metallic;
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
    // Calculate mipmap level based on roughness
    float lodRegulator = 512.0;
    float levels = float(textureQueryLevels(skybox)) - 1;
    float mx = log2(material.roughness * lodRegulator + 1) / log2(lodRegulator);
    float mipmapLevel = mx * levels;
    if (mipmapLevel > 6)
        mipmapLevel = 6;

    // Calculate reflection dir
    vec3 reflectDir = reflect(-viewDir, normal);
    //vec3 mixed = mix(normal, reflectDir, material.roughness);
    //vec3 mixed = mix(reflectDir, normal, material.roughness);
    vec3 mixed = reflectDir;

    // Get color from env map
    vec4 color = textureLod(skybox, mixed, mipmapLevel) + textureLod(skysphere, mixed.xy, mipmapLevel);

    // Set lightDir
    vec3 lightDir = reflectDir;
    //vec3 lightDir = normal;
    float NdotL   = max(dot(normal, lightDir), 0.0);
    color *= NdotL;

    // Calculate final light color
    return CalcLight(color.rgb, normal, lightDir, viewDir, material, 0.0);
}

// Calculates the color of directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    // Color
    vec3 color = CalcLight(light.color, normal, lightDir, viewDir, material, shadow);
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
