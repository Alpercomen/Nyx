#version 330 core
#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float decay;
};

in vec3 vNormalW;
in vec3 vWorldPos;

out vec4 FragColor;

uniform vec3 uCameraPos;
uniform vec3 uAtmoColor;
uniform float uAtmoIntensity;
uniform float uRimPower;        // try 3.0 - 6.0
uniform float uLightSoftness;   // try 0.2 - 0.6
uniform float uAlphaMultiplier; // try 0.15 - 0.5

uniform int uDirLightCount;
uniform int uPointLightCount;
uniform DirectionalLight uDirectionalLights[MAX_DIR_LIGHTS];
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

float Saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

vec3 ComputeLighting(vec3 N, vec3 worldPos)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < uDirLightCount; ++i)
    {
        vec3 L = normalize(-uDirectionalLights[i].direction);

        // Soft day-side contribution
        float lit = smoothstep(-uLightSoftness, 1.0, dot(N, L));
        result += uDirectionalLights[i].color * lit * uDirectionalLights[i].intensity;
    }

    for (int i = 0; i < uPointLightCount; ++i)
    {
        PointLight light = uPointLights[i];

        vec3 toL = light.position - worldPos;
        float d = length(toL);
        vec3 L = toL / max(d, 1e-6);

        float attenuation = 1.0 / (1.0 + light.decay * d * d);
        attenuation *= Saturate(1.0 - d / light.range);

        float lit = smoothstep(-uLightSoftness, 1.0, dot(N, L));
        result += light.color * lit * light.intensity * attenuation;
    }

    return result;
}

void main()
{
    vec3 N = normalize(vNormalW);
    vec3 V = normalize(uCameraPos - vWorldPos);

    // Strong only near the limb
    float rim = 1.0 - Saturate(dot(N, V));
    rim = pow(rim, uRimPower);

    // Light only on the lit side, but softly
    vec3 lightAccum = ComputeLighting(N, vWorldPos);

    // Atmosphere color
    vec3 color = uAtmoColor * lightAccum * rim * uAtmoIntensity;

    // Alpha from rim, not from full color brightness
    float alpha = rim * uAlphaMultiplier;
    alpha *= Saturate(max(max(lightAccum.r, lightAccum.g), lightAccum.b));

    FragColor = vec4(color, alpha);
}