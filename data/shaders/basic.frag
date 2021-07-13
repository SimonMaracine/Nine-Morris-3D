#version 430 core

in vec2 v_texture_coordinate;

out vec4 fragment_color;

uniform sampler2D u_diffuse;

void main() {
    fragment_color = texture(u_diffuse, v_texture_coordinate);
}
