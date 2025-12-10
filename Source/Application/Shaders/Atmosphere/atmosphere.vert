#version 450 core

layout(location = 0) in vec3 aPosition;   // unit-sphere or atmosphere-sphere vertices

uniform mat4 uModel;   // model for the atmosphere shell
uniform mat4 uView;
uniform mat4 uProj;

out VS_OUT 
{
    vec3 worldPos;
} vs_out;

void main()
{
    vec4 world = uModel * vec4(aPosition, 1.0);
    vs_out.worldPos = world.xyz;
    gl_Position = uProj * uView * world;
}