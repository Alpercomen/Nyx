#version 330 core

in vec2 vTexCoord;
in vec3 vNormal;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform vec3 uBaseColor;

void main()
{
    vec3 baseColor = uBaseColor;

    if (uHasTexture)
        baseColor = texture(uTexture, vTexCoord).rgb;

    FragColor = vec4(baseColor, 1.0);
}