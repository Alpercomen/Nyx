#version 330 core

// outputs
out vec3 nearPoint;
out vec3 farPoint;
out mat4 fragView;
out mat4 fragProj;
out float near;
out float far;

// uniforms
uniform mat4 uView;
uniform mat4 uProj;
uniform float uNear;
uniform float uFar;
uniform vec3  uCameraPos;   // kept for the fragment stage

// Fullscreen NDC quad (no VBO)
const vec2 quad[4] = vec2[](
    vec2(-1.0, -1.0),  // bottom-left
    vec2( 1.0, -1.0),  // bottom-right
    vec2(-1.0,  1.0),  // top-left
    vec2( 1.0,  1.0)   // top-right
);

// Unproject NDC -> world
vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 proj)
{
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(proj);
    vec4 p = viewInv * projInv * vec4(x, y, z, 1.0);
    return p.xyz / p.w;
}

void main()
{
    vec2 ndc = quad[gl_VertexID];

    // Rays through the near/far planes in world space
    nearPoint = UnprojectPoint(ndc.x, ndc.y, 0.0, uView, uProj);
    farPoint  = UnprojectPoint(ndc.x, ndc.y, 1.0, uView, uProj);

    fragView = uView;
    fragProj = uProj;
    near = uNear;
    far  = uFar;

    // Draw a screen-aligned quad
    gl_Position = vec4(ndc, 0.0, 1.0);
}