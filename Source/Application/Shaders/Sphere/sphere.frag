#version 330 core

in float vHeight;
in vec3 fragTopColor;
in vec3 fragBotColor;

out vec4 FragColor;

void main()
{
    // Remap height from [-1, 1] to [0, 1]
    float height = vHeight * 0.5 + 0.5;

    // Example gradient colors
    vec3 bottomColor = fragBotColor;
    vec3 topColor = fragTopColor;

    vec3 finalColor = mix(bottomColor, topColor, height);
    FragColor = vec4(finalColor, 1.0);
}