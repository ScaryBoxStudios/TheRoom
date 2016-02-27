#version 330
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform usampler2D gMatIdx;
uniform ivec2 gScreenSize;
uniform vec3 viewPos;

uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;

struct Material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct MaterialProperties
{
    float shininess;
};

layout (std140) uniform MaterialDataBlock
{
    MaterialProperties materialProps[16];
};

// General light properties
struct LightProps
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    LightProps properties;
    vec3 direction;
};

// Point light
struct PointLight
{
    LightProps properties;
    AttenuationProps attProps;
    vec3 position;
};

// Spot light
struct SpotLight
{
    LightProps properties;
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

// Internal func used by other Calc Light functions
vec3 CalcLight(LightProps lightProps, vec3 normal, vec3 lightDir, vec3 viewDir, Material material, float shadowFactor)
{
    // Diffuse shading (Lambertian reflectance)
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading (Phong model)
    float spec = CalcBlinnPhongSpec(normal, lightDir, viewDir, material.shininess);
    // Combine results
    vec3 ambient  = lightProps.ambient  * material.diffuse;
    vec3 diffuse  = lightProps.diffuse  * diff * material.diffuse * (1.0 - shadowFactor);
    vec3 specular = lightProps.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// Calculates the color of directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    // Color
    vec3 color = CalcLight(light.properties, normal, lightDir, viewDir, material, shadow);
    return color;
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Color
    vec3 color = CalcLight(light.properties, normal, lightDir, viewDir, material, 0.0);
    // Attenuation
    float attenuation = CalcAttenuationValue(light.attProps, light.position, fragPos);
    return color * attenuation;
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Color
    vec3 color = CalcLight(light.properties, normal, lightDir, viewDir, material, 0.0);
    // Attenuation
    float attenuation = CalcAttenuationValue(light.attProps, light.position, fragPos);
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    return color * attenuation * intensity;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightPos, vec3 fragPos)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // When the projected coord is further than the light's far plane
    if (projCoords.z > 1.0)
        return 0.0;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 lightDir = normalize(lightPos - fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // Return shadow factor
    return shadow;
}

uniform PointLight pLight;
uniform DirLight dirLight;
uniform int lMode;

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
    material.diffuse = Diffuse;
    material.specular = vec3(Specular);
    material.shininess = materialProps[MatIdx].shininess;

    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Calculate fragment shadow
    float shadow = ShadowCalculation(FragPosLightSpace, norm, -dirLight.direction, FragPos);

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
