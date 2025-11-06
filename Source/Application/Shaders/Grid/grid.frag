#version 330 core

in vec3 nearPoint;
in vec3 farPoint;
in mat4 fragView;
in mat4 fragProj;
in float near;
in float far;

out vec4 FragColor;

uniform vec3 uCameraPos;

vec4 grid(vec3 fragPos3D, float scale)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 g = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(g.x, g.y);
    float minz = min(derivative.y, 1.0);
    float minx = min(derivative.x, 1.0);
    float visibility = 1.0 - min(line, 1.0);
    vec3 base = mix(vec3(0.0), vec3(0.4), visibility);
    vec3 color = base;

    float alpha = max(max(color.x, color.y), color.z);
    return vec4(color, alpha);
}

float computeDepth(vec3 pos)
{
    vec4 clip = fragProj * fragView * vec4(pos, 1.0);
    return clip.z / clip.w;
}

float computeLinearDepth(vec3 pos)
{
    vec4 clip = fragProj * fragView * vec4(pos, 1.0);
    float ndcZ = (clip.z / clip.w) * 2.0 - 1.0;
    float lin = (2.0 * near * far) / (far + near - ndcZ * (far - near));
    return lin / far;
}

void main()
{
    // Intersect the view ray with the XZ plane passing through the camera (y = uCameraPos.y)
    float denom = (farPoint.y - nearPoint.y);
    float t = (-nearPoint.y - uCameraPos.y) / denom;

    if (t < 0.0) discard;

    // World-space intersection point on the grid plane
    vec3 worldPos = nearPoint + t * (farPoint - nearPoint);

    // Evaluate the grid pattern relative to where camera is. Modulus by a number so it doesn't get distorted.
    vec3 rel = worldPos + mod(uCameraPos, 1000);

    gl_FragDepth = computeDepth(worldPos);
    float linearDepth = computeLinearDepth(worldPos);
    float fading = exp(-linearDepth * 1000.0);

    vec4 g1 = grid(rel, 10.0);
    vec4 g2 = grid(rel, 100.0);
    vec4 finalColor = (g1 + g2) * fading;

    FragColor = finalColor;
}