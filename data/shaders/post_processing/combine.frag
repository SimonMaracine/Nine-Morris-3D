#version 430 core

in vec2 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;

uniform sampler2D u_screen_texture;
uniform sampler2D u_bright_texture;

const float strength = 0.4;

void main() {
    const vec4 screen_color = texture(u_screen_texture, v_texture_coordinate);
    const vec4 bright_color = texture(u_bright_texture, v_texture_coordinate);

    fragment_color = screen_color + bright_color * strength;
}
