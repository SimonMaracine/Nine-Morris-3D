#version 430 core

in vec2 v_texture_coordinate;
in flat float v_texture_index;

layout(location = 0) out vec4 fragment_color;

uniform sampler2D u_texture[8];

void main() {
    const int index = int(v_texture_index);
    fragment_color = texture(u_texture[index], v_texture_coordinate);
}
