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
    float range;    // Max distance
    float decay;    // Quadratic fallout control
};

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;
in float vFragDepth;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform vec3 uBaseColor;
uniform vec3 uEmissiveColor;
uniform float uEmissiveStrength;
uniform float uFarPlane;
uniform int uDirLightCount;
uniform int uPointLightCount;
uniform DirectionalLight uDirectionalLights[MAX_DIR_LIGHTS];
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

vec3 ComputeDirectionalLight(vec3 normal) {
    vec3 result = vec3(0.0);
    vec3 N = normalize(normal);

    for (int i = 0; i < uDirLightCount; ++i) {
        vec3 L = normalize(-uDirectionalLights[i].direction);

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = uDirectionalLights[i].color * uDirectionalLights[i].intensity * diff;

        vec3 ambient = 0.0 * uDirectionalLights[i].color;

        result += ambient + diffuse;
    }

    return result;
}

vec3 ComputePointLights(vec3 fragPos, vec3 normal)
{
    vec3 result = vec3(0.0);
    vec3 N = normalize(normal);

    for (int i = 0; i < uPointLightCount; ++i)
    {
        PointLight light = uPointLights[i];
        vec3 L = light.position - fragPos;
        float distance = length(L);
        L = normalize(L);

        float attenuation = 1.0 / (1.0 + light.decay * distance * distance);
        attenuation *= clamp(1.0 - (distance / light.range), 0.0, 1.0);

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = light.color * light.intensity * diff * attenuation;

        result += diffuse;
    }

    return result;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lighting = ComputeDirectionalLight(normal); // directional lights
    lighting += ComputePointLights(vFragPos, normal); // point lights
    
    vec3 baseColor = uBaseColor;

    if (uHasTexture) 
        baseColor *= texture(uTexture, vUV).rgb;

    vec3 glowBlend = mix(baseColor, uEmissiveColor, uEmissiveStrength);
    vec3 finalColor = glowBlend + baseColor * lighting;

    FragColor = vec4(finalColor, 1.0);

    float c = 1.0 / log2(uFarPlane + 1.0);
    gl_FragDepth = log2(vFragDepth) * c;
}