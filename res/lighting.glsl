#module lighting

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

float BeckmannDistribution(float smoothness, float NdotH)
{
    float pi    = 3.14159265;

    float smoothness2 = smoothness * smoothness;
    float NdotH2     = NdotH * NdotH;
    float r1 = 1.0 / (pi * smoothness2 * pow(NdotH, 4.0));
    float r2 = (NdotH2 - 1.0) / (smoothness2 * NdotH2);
    return r1 * exp(r2);
}

float Fresnel(float F0, float VdotH)
{
    float F = pow(1.0 - VdotH, 5.0);
    F *= (1.0 - F0);
    F += F0;
    return F;
    //return mix(F0, 1, pow(1.0 - VdotH, 5.0));
}

// Calculates specular intensity according to the Cook - Torrance model
float CalcCookTorSpec(vec3 normal, vec3 lightDir, vec3 viewDir, float roughness, float F0)
{
    // Calculate intermediary values
    vec3 halfVector = normalize(lightDir + viewDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float NdotH = max(dot(normal, halfVector), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0); // Note: this could also be NdotL, which is the same value
    float VdotH = max(dot(viewDir, halfVector), 0.0);
    float pi    = 3.14159265;
    float smoothness = max(1.0 - roughness, 0.01);

    float specular = 0.0;
    if(NdotL > 0.0)
    {
        float G = GeometricalAttenuation(NdotH, NdotV, VdotH, NdotL);
        float D = BeckmannDistribution(smoothness, NdotH);
        float F = Fresnel(F0, VdotH);

        specular = (D * F * G) / (NdotV * NdotL * pi);
    }
    return specular;
}

float DiffuseLambert(vec3 normal, vec3 lightDir)
{
    return max(dot(normal, lightDir), 0.0);
}

// Internal func used by other Calc Light functions
vec3 CalcLight(vec3 lightColor, vec3 normal, vec3 lightDir, vec3 viewDir, Material material, float shadowFactor, vec3 ambient)
{
    // Helper variables
    vec3  baseColor = material.diffuse;
    vec3  specColor = material.specular;
    vec3  emissive  = material.emissive;
    float roughness = material.roughness;
    float fresnel   = material.fresnel;
    float metallic  = material.metallic;
    float pi        = 3.14159265;

    // Calculate contribution based on metallicity
    vec3 diffuseColor  = baseColor - baseColor * metallic;
    vec3 specularColor = mix(vec3(0.00), baseColor, metallic);

    // Lambertian reflectance
    float diff = DiffuseLambert(normal, lightDir);

    // Specular shading (Cook-Torrance model)
    float reflectance = CalcCookTorSpec(normal, lightDir, viewDir, roughness, fresnel);

    // Combine results
    vec3 diffuse  = diffuseColor * diff;
    vec3 specular = reflectance * specularColor;
    vec3 result   = emissive + diffuse + specular;
    vec3 sum      = ambient * result + lightColor * result;
    return pi * sum * (1.0 - shadowFactor) / 2;
}

// Calculates the color of directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material, float shadow, vec3 ambient)
{
    vec3 lightDir = normalize(-light.direction);
    // Color
    vec3 color = CalcLight(light.color, normal, lightDir, viewDir, material, shadow, ambient);
    return color;
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material, vec3 ambient)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Color
    vec3 color = CalcLight(light.color, normal, lightDir, viewDir, material, 0.0, ambient);
    // Attenuation
    float attenuation = CalcAttenuationValue(light.attProps, light.position, fragPos);
    return color * attenuation;
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material, vec3 ambient)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Color
    vec3 color = CalcLight(light.color, normal, lightDir, viewDir, material, 0.0, ambient);
    // Attenuation
    float attenuation = CalcAttenuationValue(light.attProps, light.position, fragPos);
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    return color * attenuation * intensity;
}
