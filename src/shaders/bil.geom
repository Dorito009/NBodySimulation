#version 430

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) uniform mat4 u_view;
layout(location = 1) uniform mat4 u_proj;
layout(location = 2) uniform mat3 u_camBasis;  // Camera basis matrix: columns = right, up, forward

const float halfSize = 0.1;

layout(location = 0) in vec3 v_worldPos[];
layout(location = 1) in vec3 v_vel[];

layout(location = 0) out vec2 f_uv;
layout(location = 1) out vec3 f_vel;

void main() {
    vec3 center = v_worldPos[0];

    // Extract camera right and up vectors from the basis matrix
    vec3 camRight = normalize(u_camBasis[0]); // Column 0 = right
    vec3 camUp    = normalize(u_camBasis[1]); // Column 1 = up

    vec3 halfRight = halfSize * camRight;
    vec3 halfUp    = halfSize * camUp;

    // Four corners of the billboard (triangle strip order)
    vec3 pos0 = center - halfRight - halfUp;
    vec3 pos1 = center + halfRight - halfUp;
    vec3 pos2 = center - halfRight + halfUp;
    vec3 pos3 = center + halfRight + halfUp;

    f_vel = v_vel[0]; // Pass velocity to fragment shader

    f_uv = vec2(0.0, 0.0);
    gl_Position = u_proj * u_view * vec4(pos0, 1.0);
    EmitVertex();

    f_uv = vec2(1.0, 0.0);
    gl_Position = u_proj * u_view * vec4(pos1, 1.0);
    EmitVertex();

    f_uv = vec2(0.0, 1.0);
    gl_Position = u_proj * u_view * vec4(pos2, 1.0);
    EmitVertex();

    f_uv = vec2(1.0, 1.0);
    gl_Position = u_proj * u_view * vec4(pos3, 1.0);
    EmitVertex();

    EndPrimitive();
}
