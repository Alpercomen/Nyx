#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec2 vTexCoord;
out vec3 vNormal;

void main()
{
    vTexCoord = aTexCoord;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;

    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
}