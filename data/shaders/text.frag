#version 430 core

in vec2 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;

uniform sampler2D u_bitmap;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_bitmap, v_texture_coordinate).r);
    fragment_color = vec4(1.0, 0.0, 0.0, 1.0) * sampled;
}
