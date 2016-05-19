#module lighting
#extension GL_ARB_texture_query_levels : enable
#extension GL_ARB_gpu_shader5 : enable

uniform samplerCube skybox;
uniform samplerCube irrMap;

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
    float transparency;
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

float GeometricalAttenuation(float NdotH, float NdotV, float VdotH, float NdotL)
{
    float NH2 = 2.0 * NdotH / VdotH;
    float g1  = NH2 * NdotV;
    float g2  = NH2 * NdotL;
    return min(1.0, min(g1, g2));
}

float BeckmannDistribution(float roughness, float NdotH)
{
    float a  = roughness * roughness;
    float NdotH2  = NdotH * NdotH;
    float aNdotH2 = a * NdotH2;
    return exp((NdotH2 - 1.0) / (0.01 + aNdotH2)) / (0.01 + pi * aNdotH2 * NdotH2);
}

// Slick's approximation
// Pass the reflected color at normal angle directly
float Fresnel(float F0, float VdotH)
{
    float u = 1.0 - VdotH;
    float u5 = u * u * u * u * u;
    return mix(u5, 1.0, F0);
}

vec3 BRDF(vec3 N, vec3 L, vec3 V, vec3 baseColor, float metallic, float roughness, float reflectivity, vec3 cdiff, vec3 cspec)
{
    vec3  H     = normalize(L + V);
    float NdotL = saturate(dot(N, L));
    float NdotH = max(1e-5, dot(N, H));
    float NdotV = max(1e-5, dot(N, V));
    float VdotH = clamp(dot(V, H), 1e-5, 1.0);

    // Calculate fresnel
    float F = Fresnel(reflectivity, VdotH);

    // Specular and diffuse contributions
    vec3 Ks = F * cspec;
    vec3 Kd = vec3(1.0 - F); // * (1.0 - transparency);

    // Calculate Distribution and Geometrical Attenuation
    float G = GeometricalAttenuation(NdotH, NdotV, VdotH, NdotL);
    float D = BeckmannDistribution(roughness, NdotH);

    // Final results
    vec3 d = Kd * cdiff / pi;
    vec3 s = 0.25 * Ks * D * G / (1.0 + NdotV * NdotL);
    return d + s;
}

// BRDF simplification that calculates diffuse and specular colors instead of getting them as input
vec3 BRDF(vec3 N, vec3 L, vec3 V, vec3 baseColor, float metallic, float roughness, float reflectivity)
{
    // Calc colors
    vec3 cdiff = (1 - metallic) * baseColor;
    vec3 cspec = mix(vec3(1.0), baseColor, metallic);

    return BRDF(N, L, V, baseColor, metallic, roughness, reflectivity, cdiff, cspec);
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), float(bitfieldReverse(i)) * 2.3283064365386963e-10);
}

// Computes the exact mip-map to reference for the specular contribution.
// Accessing the proper mip-map allows us to approximate the integral for this
// angle of incidence on the current object.
float compute_lod(uint NumSamples, float NoH, float roughness)
{
    // Distribution
    float D = BeckmannDistribution(NoH, roughness);

    uvec2 Dimensions = uvec2(2048, 2048);
    return 0.5 * (log2(float(Dimensions.x * Dimensions.y) / NumSamples) - log2(D));
}

float randAngle()
{
    uint x = uint(gl_FragCoord.x);
    uint y = uint(gl_FragCoord.y);
    return (30u * x ^ y + 10u * x * y);
}

vec3 MakeSample(float Theta, float Phi)
{
    Phi += randAngle();
    float SineTheta = sin(Theta);

    float x = cos(Phi) * SineTheta;
    float y = sin(Phi) * SineTheta;
    float z = cos(Theta);

    return vec3(x, y, z);
}

// Calculates the specular influence for a surface at the current fragment
// location. This is an approximation of the lighting integral itself.
vec3 radiance(vec3 N, vec3 V, vec3 baseColor, float metallic, float roughness, float reflectivity)
{
    // Helper variables
    vec3 ZAxis = vec3(0.0, 0.0, 1.0);
    vec3 XAxis = vec3(1.0, 0.0, 0.0);
    float pi2  = pi * pi;

    // Precalculate rotation for +Z Hemisphere to microfacet normal.
    vec3 UpVector = abs(N.z) < 0.999 ? ZAxis : XAxis;
    vec3 TangentX = normalize(cross( UpVector, N ));
    vec3 TangentY = cross(N, TangentX);

    // Note: I ended up using abs() for situations where the normal is
    // facing a little away from the view to still accept the approximation.
    // I believe this is due to a separate issue with normal storage, so
    // you may only need to saturate() each dot value instead of abs().
    float NoV = abs(dot(N, V));

    // Approximate the integral for lighting contribution.
    vec3 fColor = vec3(0.0);
    const uint NumSamples = 10;
    for (uint i = 0; i < NumSamples; ++i)
    {
        vec2 Xi = Hammersley(i, NumSamples);

        // Distribution
        float r2 = roughness * roughness;
        float theta = atan(sqrt((r2 * Xi.x) / (1.0 - Xi.x)));
        float phi   = pi2 * Xi.y;
        vec3 Li = MakeSample(theta, phi);

        // Calculate Color
        vec3 H      = normalize(Li.x * TangentX + Li.y * TangentY + Li.z * N);
        vec3 L      = normalize(-reflect(V, H));
        float NoH   = abs(dot(N, H));
        float lod   = compute_lod(NumSamples, NoH, roughness);
        vec3 LColor = textureLod(skybox, L, lod).rgb;

        // BRDF
        vec3 brdfResult = BRDF(N, L, V, baseColor, metallic, roughness, reflectivity);

        fColor += brdfResult * LColor;
    }

    // Average the results
    return fColor / float(NumSamples);
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

// *NOTE*: Only for testing purposes. This should be removed
float CalcMipmapLevel(float roughness)
{
    if (roughness <= 0.2)
        return 0;
    else if (roughness <= 0.3)
        return 7;
    else if (roughness <= 0.4)
        return 8;
    else if (roughness <= 0.6)
        return 9;
    else
        return 10;
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
    //float mipmapLevel = CalcMipmapLevel(roughness);

    // Get radiance and irradiance colors
    //vec3 rad = textureLod(skybox, reflectDir, mipmapLevel).rgb;
    //vec3 rad = radiance(normal, viewDir, baseColor, metallic, roughness, reflectivity);
    vec3 rad = texture(skybox, reflectDir).rgb;
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
