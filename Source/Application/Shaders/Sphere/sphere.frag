#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform vec3 uBaseColor;  // Base color tint

void main()
{
    vec3 color;

    if (uHasTexture)
    {
        vec3 texColor = texture(uTexture, vUV).rgb;
        color = texColor * uBaseColor; // Apply tint
    }
    else
    {
        color = uBaseColor; // No texture, use base color
    }

    FragColor = vec4(color, 1.0);
}