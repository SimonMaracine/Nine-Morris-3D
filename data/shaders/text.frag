#version 430 core

in vec2 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;

uniform sampler2D u_bitmap;
uniform vec3 u_color;

const float width = 0.3;
const float edge = 0.2;

void main() {
    float dist = 1.0 - texture(u_bitmap, v_texture_coordinate).r;
    float alpha = 1.0 - smoothstep(width, width + edge, dist);
    fragment_color = vec4(u_color, alpha);
}
