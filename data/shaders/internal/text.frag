#version 430 core

in vec2 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;

uniform sampler2D u_bitmap;
uniform vec3 u_color;
uniform float u_border_width;  // 0.5
uniform vec2 u_offset;  // vec2(-0.003, -0.003)

const float width = 0.3;
const float edge = 0.2;

const float border_edge = 0.4;

const vec3 outline_color = vec3(0.2, 0.2, 0.2);

void main() {
    const float dist = 1.0 - texture(u_bitmap, v_texture_coordinate).r;
    const float alpha = 1.0 - smoothstep(width, width + edge, dist);

    const float dist2 = 1.0 - texture(u_bitmap, v_texture_coordinate + u_offset).r;
    const float outline_alpha = 1.0 - smoothstep(u_border_width, u_border_width + border_edge, dist2);

    const float overall_alpha = alpha + (1.0 - alpha) * outline_alpha;
    const vec3 overall_color = mix(outline_color, u_color, alpha / overall_alpha);

    fragment_color = vec4(overall_color, overall_alpha);
}
