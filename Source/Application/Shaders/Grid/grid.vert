#version 330 core

out vec3 nearPoint;
out vec3 farPoint;
out mat4 fragView;
out mat4 fragProj;
out float near;
out float far;

uniform mat4 uView;
uniform mat4 uProj;
uniform float uNear;
uniform float uFar;

vec3 gridPlane[4] = vec3[](
    vec3(-1.0, -1.0, 0.0), // bottom-left
    vec3( 1.0, -1.0, 0.0), // bottom-right
    vec3(-1.0,  1.0, 0.0), // top-left
    vec3( 1.0,  1.0, 0.0)  // top-right
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection)
{
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);

    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    vec3 position = gridPlane[gl_VertexID].xyz;

    nearPoint = UnprojectPoint(position.x, position.y, 0.0, uView, uProj).xyz;
    farPoint = UnprojectPoint(position.x, position.y, 1.0, uView, uProj).xyz;

    fragView = uView;
    fragProj = uProj;

    near = uNear;
    far = uFar;

    gl_Position = vec4(position, 1.0);
}