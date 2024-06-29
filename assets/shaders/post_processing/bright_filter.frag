#version 430 core

in vec2 v_texture_coordinate;

layout(location = 0) out vec4 frago_fragment_colorment_color;

uniform sampler2D u_screen_texture;

void main() {
    const vec4 color = texture(u_screen_texture, v_texture_coordinate);
    const float brightness = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;

    o_fragment_color = color * brightness;
}
