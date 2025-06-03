#version 430

layout(location = 0) in vec2 f_uv;
layout(location = 1) in vec3 f_vel;
layout(location = 0) out vec4 outColor;

void main() {
    float speed = length(f_vel);  // Compute velocity magnitude

    // Map speed to color (you can tweak this)
    vec3 color = mix(vec3(0.0, 0.0, 1.0),  // Blue = slow
                     vec3(1.0, 0.0, 0.0),  // Red = fast
                     clamp(speed / 10.0, 0.0, 1.0)); // Normalize

    float d = distance(f_uv, vec2(0.5, 0.5));
    if (d > 0.5) discard;

    outColor = vec4(color, 0.5);
}