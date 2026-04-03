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

uniform float uHaloIntensity;   // start 1.0
uniform float uHaloAlpha;       // start 0.35
uniform float uRimStart;        // start 0.80
uniform float uRimEnd;          // start 1.00
uniform float uLightSoftness;   // start 0.25

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

    // Important: use abs so back/front orientation does not break the rim
    float ndotv = abs(dot(N, V));
    float fresnel = 1.0 - Saturate(ndotv);

    // Narrow band only near the atmosphere-shell silhouette
    float haloMask = smoothstep(uRimStart, uRimEnd, fresnel);

    // Keep lighting on the true outward normal
    vec3 lightAccum = ComputeLighting(N, vWorldPos);
    float lightMask = Saturate(max(max(lightAccum.r, lightAccum.g), lightAccum.b));

    vec3 color = uAtmoColor * lightAccum * haloMask * uHaloIntensity;
    float alpha = haloMask * lightMask * uHaloAlpha;

    FragColor = vec4(color, alpha);
}