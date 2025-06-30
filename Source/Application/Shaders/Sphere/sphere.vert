#version 330 core

layout(location = 0) in vec3 aPos;

out float vHeight; // send Y value to fragment shader
out vec3 fragTopColor;
out vec3 fragBotColor;
// Just pass the values to fragment shader

uniform mat4 uMVP;
uniform vec3 topColor;
uniform vec3 botColor;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    vHeight = aPos.y; // pass Y value
    fragTopColor = topColor;
    fragBotColor = botColor;
}