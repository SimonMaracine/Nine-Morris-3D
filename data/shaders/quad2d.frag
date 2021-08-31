#version 430 core

in vec2 v_texture_coordinate;

out vec4 fragment_color;

uniform sampler2D u_texture;

void main() {
    fragment_color = texture(u_texture, v_texture_coordinate);
}
