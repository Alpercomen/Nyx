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

uniform vec3  uAtmoColor;
uniform float uAtmoIntensity;
uniform float uRimPower;
uniform float uLightPower;

uniform int uDirLightCount;
uniform int uPointLightCount;
uniform DirectionalLight uDirectionalLights[MAX_DIR_LIGHTS];
uniform PointLight uPointLights[MAX_POINT_LIGHTS];


vec3 ComputeLightFactor(vec3 N, vec3 worldPos)
{
    float lit = 0.0;

    for (int i = 0; i < uDirLightCount; ++i)
    {
        vec3 L = normalize(-uDirectionalLights[i].direction);
        float ndotl = max(dot(N, L), 0.0);
        lit += ndotl * uDirectionalLights[i].intensity;
    }

    for (int i = 0; i < uPointLightCount; ++i)
    {
        PointLight light = uPointLights[i];
        vec3 toL = light.position - worldPos;
        float d = length(toL);
        vec3 L = toL / max(d, 1e-6);

        float attenuation = 1.0 / (1.0 + light.decay * d * d);
        attenuation *= clamp(1.0 - (d / light.range), 0.0, 1.0);

        float ndotl = max(dot(N, L), 0.0);
        lit += ndotl * light.intensity * attenuation;
    }

    lit = clamp(lit, 0.0, 1.0);
    return vec3(lit);
}

void main()
{
    vec3 N = normalize(vNormalW);
    vec3 V = normalize(uCameraPos - vWorldPos);

    float rim = pow(clamp(1.0 - dot(N, V), 0.0, 1.0), uRimPower);
    float lightFactor = pow(ComputeLightFactor(N, vWorldPos).x, uLightPower);
    float alpha = rim * lightFactor * uAtmoIntensity;

    FragColor = vec4(uAtmoColor * alpha, alpha);
}