#version 430 core

in vec2 v_texture_coordinate;

layout(location = 0) out vec4 fragment_color;

layout(binding = 0) uniform sampler2D u_bitmap;
uniform vec3 u_color;
uniform float u_border_width;  // 0.5
uniform vec2 u_offset;  // vec2(-0.003, -0.003)

const float WIDTH = 0.3;
const float EDGE = 0.2;

const float BORDER_EDGE = 0.4;
const vec3 OUTLINE_COLOR = vec3(0.1, 0.1, 0.1);

void main() {
    const float dist = 1.0 - texture(u_bitmap, v_texture_coordinate).r;
    const float alpha = 1.0 - smoothstep(WIDTH, WIDTH + EDGE, dist);

    const float dist2 = 1.0 - texture(u_bitmap, v_texture_coordinate + u_offset).r;
    const float outline_alpha = 1.0 - smoothstep(u_border_width, u_border_width + BORDER_EDGE, dist2);

    const float overall_alpha = alpha + (1.0 - alpha) * outline_alpha;
    const vec3 overall_color = mix(OUTLINE_COLOR, u_color, alpha / overall_alpha);

    fragment_color = vec4(overall_color, overall_alpha);
}
