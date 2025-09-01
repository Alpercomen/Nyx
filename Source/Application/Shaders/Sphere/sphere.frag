#version 330 core
#define MAX_DIR_LIGHTS 4

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform vec3 uBaseColor;
uniform int uDirLightCount;
uniform DirectionalLight uDirectionalLights[MAX_DIR_LIGHTS];

vec3 ComputeLighting(vec3 normal) {
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

void main()
{
    vec3 baseColor = uBaseColor;
    if (uHasTexture) {
        baseColor *= texture(uTexture, vUV).rgb;
    }

    vec3 lighting = ComputeLighting(vNormal);
    vec3 finalColor = baseColor * lighting;

    FragColor = vec4(finalColor, 1.0);
}