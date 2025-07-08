#version 330 core

layout (location = 0) in vec3 aPos; // Position
layout (location = 1) in vec2 aUV;  // Texture coordinate

uniform mat4 uMVP;

out vec2 vUV; // Pass UV to fragment shader

void main()
{
    vUV = aUV; // Forward UV
    gl_Position = uMVP * vec4(aPos, 1.0);
}