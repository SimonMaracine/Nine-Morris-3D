#version 430 core

// https://prideout.net/blog/old/blog/index.html@p=54.html
// https://stackoverflow.com/questions/19036533/opengl-triangle-adjacency-indexing
// https://stackoverflow.com/questions/39402823/opengl-triangle-adjacency-calculation

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 12) out;

uniform float u_width;
uniform float u_overhang;

bool is_front_facing(vec3 V1, vec3 V2, vec3 V3) {
    return (V1.x * V2.y - V2.x * V1.y) + (V2.x * V3.y - V3.x * V2.y) + (V3.x * V1.y - V1.x * V3.y) > 0;
}

void emit_edge(vec3 P1, vec3 P2) {
    const vec3 E = vec3(P2.xy - P1.xy, 0.0) * u_overhang;
    const vec2 V = normalize(E.xy);
    const vec3 N = vec3(-V.y, V.x, 0.0) * u_width;

    gl_Position = vec4(P1 - N - E, 1.0);
    EmitVertex();
    gl_Position = vec4(P1 + N - E, 1.0);
    EmitVertex();
    gl_Position = vec4(P2 - N + E, 1.0);
    EmitVertex();
    gl_Position = vec4(P2 + N + E, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    const vec3 v0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    const vec3 v1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    const vec3 v2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    const vec3 v3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    const vec3 v4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
    const vec3 v5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

    if (is_front_facing(v0, v2, v4)) {
        if (!is_front_facing(v0, v1, v2)) {
            emit_edge(v0, v2);
        }

        if (!is_front_facing(v2, v3, v4)) {
            emit_edge(v2, v4);
        }

        if (!is_front_facing(v0, v4, v5)) {
            emit_edge(v4, v0);
        }
    }
}
