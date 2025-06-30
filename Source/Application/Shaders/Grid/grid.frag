#version 330 core

in vec3 nearPoint;
in vec3 farPoint;
in mat4 fragView;
in mat4 fragProj;
in float near;
in float far;

out vec4 FragColor;

vec4 grid(vec3 fragPos3D, float scale)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    float visibility = 1.0 - min(line, 1.0);
    vec3 baseColor = mix(vec3(0.0), vec3(0.3), visibility); // Dark background, bright grid lines
    vec3 color = baseColor;

    //z axis
    if (fragPos3D.x > -50 * minimumx && fragPos3D.x < 50 * minimumx)
        color.z = 1.0;

    //x axis
    if (fragPos3D.z > -50 * minimumz && fragPos3D.z < 50 * minimumz)
        color.x = 1.0;

    float alpha = max(max(color.x, color.y), color.z);

    return vec4(color, alpha);
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main()
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);

    if (t < 0.0)
        discard;

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);
    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = exp(-linearDepth * 10.0);

    vec4 g1 = grid(fragPos3D, 0.001);
    vec4 g2 = grid(fragPos3D, 0.01);
    vec4 finalColor = (g1 + g2) * fading;

    FragColor = vec4(finalColor); // Keep alpha 1 if you're not using blending
}