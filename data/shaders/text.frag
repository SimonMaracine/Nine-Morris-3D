#version 430 core

in vec2 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;

uniform sampler2D u_bitmap;
uniform vec3 u_color;

void main() {
    fragment_color = vec4(u_color, texture(u_bitmap, v_texture_coordinate).r);
}
