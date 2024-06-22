#version 430 core

in vec2 v_texture_coordinate;
in flat int v_index;

layout(location = 0) out vec4 fragment_color;

layout(binding = 0) uniform sampler2D u_bitmap;

const int MAX_OBJECTS = 10;

uniform vec3 u_color[MAX_OBJECTS];
uniform float u_border_width[MAX_OBJECTS];  // 0.5
uniform vec2 u_offset[MAX_OBJECTS];  // vec2(-0.003, -0.003)

const float WIDTH = 0.3;
const float EDGE = 0.2;

const float BORDER_EDGE = 0.4;
const vec3 OUTLINE_COLOR = vec3(0.1, 0.1, 0.1);

void main() {
    const float dist = 1.0 - texture(u_bitmap, v_texture_coordinate).r;
    const float alpha = 1.0 - smoothstep(WIDTH, WIDTH + EDGE, dist);

    const float dist2 = 1.0 - texture(u_bitmap, v_texture_coordinate + u_offset[v_index]).r;
    const float outline_alpha = 1.0 - smoothstep(u_border_width[v_index], u_border_width[v_index] + BORDER_EDGE, dist2);

    const float overall_alpha = alpha + (1.0 - alpha) * outline_alpha;
    const vec3 overall_color = mix(OUTLINE_COLOR, u_color[v_index], alpha / overall_alpha);

    fragment_color = vec4(overall_color, overall_alpha);
}
