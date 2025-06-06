#version 430

layout(location = 0) in vec2 f_uv;
layout(location = 1) in vec3 f_vel;
layout(location = 0) out vec4 outColor;

void main() {

    float d = length(f_uv - vec2(0.5, 0.5));
    vec3 color = vec3(1.0, 0.0, 0.0);
    if (d > 0.3) discard;

    outColor = vec4(color, 0.5);
}