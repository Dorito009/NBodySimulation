#version 430

// 1) Declare the same Body struct (32 bytes; two vec4’s)
struct Body {
    vec4 position;   // .xyz = world‐pos; .w = (unused)
    vec4 velocity;   // .xyz = world‐vel; .w = mass
};

// 2) Bind the SSBO at binding = 0 (must match glBindBufferBase(..., 0, ...))
layout(std430, binding = 0) buffer BodyBuffer {
    Body bodies[];     // array of all N bodies, in the same memory layout as CPU
};

// 3) Output the world‐space position to the geometry shader
layout(location = 0) out vec3 v_worldPos;

void main() {
    uint idx = gl_VertexID;            // 0 .. N−1 (no VBO needed)
    Body b = bodies[idx];              // read the i-th Body from the SSBO
    vec3 worldPos = b.position.xyz;    // extract vec3 position

    v_worldPos = worldPos;

    // Emit a “dummy” point; geometry shader will turn it into a quad
    gl_Position = vec4(worldPos, 1.0);
}
