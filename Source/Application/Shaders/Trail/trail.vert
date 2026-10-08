#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aAlpha;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vColor;
out float vAlpha;

void main()
{
    gl_Position = uProj * uView * vec4(aPosition, 1.0);

    vColor = aColor;
    vAlpha = aAlpha;
}