#version 430 core

in vec2 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out int id;

uniform sampler2D u_diffuse;

void main() {
    fragment_color = texture(u_diffuse, v_texture_coordinate);
    id = 19;
}
