#version 450 core

in VS_OUT {
    vec3 worldPos;
} fs_in;

out vec4 FragColor;

uniform vec3  uCameraPos;
uniform vec3  uPlanetCenter;

uniform float uPlanetRadius;
uniform float uAtmosphereRadius;

// Scattering tint (Rayleigh-optimized)
uniform vec3  uScatteringColor = vec3(0.42, 0.63, 1.0);

// Artistic controls
uniform float uDensityFalloff = 1.5;
uniform float uEdgeStrength   = 2.2;
uniform float uCenterStrength = 0.35;
uniform float uEdgePower      = 3.0;
uniform float uCenterPower    = 3.0;
uniform float uExposure       = 1.0;

// ------------------------------------------------------------
// Multiple LIGHT SOURCES (matches your engine)
// ------------------------------------------------------------
#define MAX_DIRECTIONAL_LIGHTS 8
#define MAX_POINT_LIGHTS 16

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

uniform int uDirLightCount;
uniform DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS];

uniform int uPointLightCount;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
// ------------------------------------------------------------

float saturate(float x) { return clamp(x, 0.0, 1.0); }

void main()
{
    vec3 toCenter = fs_in.worldPos - uPlanetCenter;
    float dist = length(toCenter);
    vec3 normal = toCenter / dist;

    vec3 viewDir = normalize(uCameraPos - fs_in.worldPos);

    float NdotV = saturate(dot(normal, -viewDir));

    float height = dist - uPlanetRadius;
    float shell = max(uAtmosphereRadius - uPlanetRadius, 0.001);
    float h = saturate(height / shell);

    float horizonTerm = pow(1.0 - NdotV, uEdgePower);
    float radialDensity = exp(-h * uDensityFalloff);
    float edgeScatter = horizonTerm * radialDensity;

    float centerTerm = pow(NdotV, uCenterPower) * (1.0 - h);
    float centerScatter = centerTerm;

    // ------------------------------------------------------------
    //        LIGHTING: MULTIPLE DIRECATIONAL + POINT LIGHTS
    // ------------------------------------------------------------
    float totalLight = 0.0;

    // ---- Directional Lights ----
    for (int i = 0; i < uDirLightCount; i++)
    {
        vec3 L = normalize(-uDirectionalLights[i].direction);

        float NdotL = dot(normal, L);
        float lf = smoothstep(0.0, 0.2, NdotL);

        totalLight += lf * uDirectionalLights[i].intensity;
    }

    // ---- Point Lights -----
    for (int i = 0; i < uPointLightCount; i++)
    {
        vec3 L = normalize(uPointLights[i].position - fs_in.worldPos);

        float distToLight = length(uPointLights[i].position - fs_in.worldPos);
        float attenuation = pow(saturate(1.0 - distToLight / uPointLights[i].range),
                                uPointLights[i].decay);

        float NdotL = dot(normal, L);
        float lf = smoothstep(0.0, 0.2, NdotL);

        totalLight += lf * attenuation * uPointLights[i].intensity;
    }

    totalLight = saturate(totalLight);

    float scatter = (edgeScatter * uEdgeStrength + centerScatter * uCenterStrength) * totalLight;

    scatter = saturate(scatter);

    vec3 color = uScatteringColor * scatter;
    color = vec3(1.0) - exp(-color * uExposure);

    float alpha = scatter;
    float outerFade = saturate(1.0 - h);
    alpha *= outerFade;

    if (alpha < 0.001)
        discard;

    FragColor = vec4(color, alpha);
}