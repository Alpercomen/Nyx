#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;

out vec4 FragColor;

uniform vec3 uCameraPos;
uniform vec3 uPlanetCenter;
uniform float uPlanetRadius;
uniform float uAtmosphereRadius;
uniform vec3 uScatteringColor;
uniform float uScatteringStrength;

// === Lighting ===
#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 16

// === Light System ===
uniform int uDirLightCount;
uniform int uPointLightCount;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float decay;
};

uniform DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

const float PI = 3.14159265359;

float RayleighPhase(float cosTheta) 
{
    return 3.0 / (16.0 * PI) * (1.0 + cosTheta * cosTheta);
}

float Attenuate(float dist, float range, float decay) 
{
    return pow(clamp(1.0 - dist / range, 0.0, 1.0), decay);
}

void main() {
    vec3 viewDir = normalize(vWorldPos - uCameraPos);
    vec3 normal = normalize(vWorldPos - uPlanetCenter);

    // Compute how deep into the atmosphere this pixel is
    float shellDist = length(vWorldPos - uPlanetCenter);
    float heightRatio = clamp((shellDist - uPlanetRadius) / (uAtmosphereRadius - uPlanetRadius), 0.0, 1.0);

    vec3 tint = vec3(0.0);

    // Directional Lights
    for (int i = 0; i < uDirLightCount; ++i) 
    {
        vec3 lightDir = normalize(-uDirectionalLights[i].direction);
        float cosTheta = dot(viewDir, lightDir);
        float rayleigh = RayleighPhase(cosTheta);

        float NdotL = max(dot(normal, lightDir), 0.0);
        vec3 lightColor = uDirectionalLights[i].color * uDirectionalLights[i].intensity;

        tint += rayleigh * lightColor * NdotL;
    }

    // Point Lights
    for (int i = 0; i < uPointLightCount; ++i) 
    {
        vec3 lightVec = uPointLights[i].position - vWorldPos;
        float dist = length(lightVec);
        vec3 lightDir = normalize(lightVec);
        float cosTheta = dot(viewDir, lightDir);
        float rayleigh = RayleighPhase(cosTheta);
        float NdotL = max(dot(normal, lightDir), 0.0);

        float atten = Attenuate(dist, uPointLights[i].range, uPointLights[i].decay);
        vec3 lightColor = uPointLights[i].color * uPointLights[i].intensity * atten;

        tint += rayleigh * lightColor * NdotL;
    }

    // === Day-side camera-facing glow ===
    for (int i = 0; i < uDirLightCount; ++i) 
    {
        vec3 lightDir = normalize(-uDirectionalLights[i].direction);
        vec3 lightColor = uDirectionalLights[i].color * uDirectionalLights[i].intensity;

        float NdotL = max(dot(normal, lightDir), 0.0);       // Lit side
        float VdotN = max(dot(normal, -viewDir), 0.0);       // Facing camera
        float softness = pow(VdotN, 2.0);

        vec3 surfaceGlow = lightColor * NdotL * softness;

        tint += surfaceGlow * uScatteringColor * 0.5;
    }

    vec3 finalColor = tint * uScatteringColor * uScatteringStrength;
    finalColor = clamp(finalColor, 0.0, 1.0);

    // Let intensity scale alpha directly
    float baseAlpha = pow(heightRatio, 0.8);
    float alpha = baseAlpha * clamp(uScatteringStrength * 0.1, 0.0, 1.0); // Clamp for sanity

    FragColor = vec4(finalColor, alpha);
}