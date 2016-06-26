#module brdf
#include math

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

vec3 BRDF(vec3 N, vec3 L, vec3 V, vec3 baseColor, float metallic, float roughness, float reflectivity)
{
    vec3  H     = normalize(L + V);
    float NdotL = saturate(dot(N, L));
    float NdotH = max(1e-5, dot(N, H));
    float NdotV = max(1e-5, dot(N, V));
    float VdotH = clamp(dot(V, H), 1e-5, 1.0);

    // Calc colors
    vec3 cdiff = (1 - metallic) * baseColor;
    vec3 cspec = mix(vec3(1.0), baseColor, metallic);

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
    vec3 s = Ks * D * G / (1e-5 + 4 * NdotV * NdotL);
    return d + s;
}

vec3 EnvBRDF(vec3 N, vec3 L, vec3 V, vec3 baseColor, float metallic, float roughness, float reflectivity, vec3 irr, vec3 rad)
{
    vec3  H     = normalize(L + V);
    float NdotL = saturate(dot(N, L));
    float NdotH = max(1e-5, dot(N, H));
    float NdotV = max(1e-5, dot(N, V));
    float VdotH = clamp(dot(V, H), 1e-5, 1.0);

    // Calc colors
    vec3 cdiff = (1 - metallic) * baseColor;
    vec3 cspec = mix(vec3(1.0), baseColor, metallic);

    // Calculate fresnel
    float F = Fresnel(reflectivity, VdotH);

    // Calculate Distribution and Geometrical Attenuation
    float G = GeometricalAttenuation(NdotH, NdotV, VdotH, NdotL);

    // Final results
    vec3 d = (1 - F) * cdiff * irr / pi;
    vec3 s = cspec * rad * F * G;
    return d + s * NdotL;
}
