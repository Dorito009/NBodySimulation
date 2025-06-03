#version 430

layout(location = 0) in vec2 f_uv;
layout(location = 0) out vec4 outColor;

void main() {
    // Compute distance from the center of the quad (0.5, 0.5)
    float d = distance(f_uv, vec2(0.5, 0.5));

    // If d > 0.5, we are outside the circle → discard (or alpha=0)
    // If d ≤ 0.5, inside circle → white opaque (or you can smooth edge)
    if (d > 0.5) {
        discard;
    }

    // You can smooth the edge with, e.g., smoothstep:
    // float alpha = smoothstep(0.5, 0.48, d);
    // outColor = vec4(1.0, 1.0, 1.0, alpha);

    // For a hard circle:
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}