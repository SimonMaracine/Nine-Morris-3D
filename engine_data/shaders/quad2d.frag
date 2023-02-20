#version 430 core

in vec2 v_texture_coordinate;
in flat float v_texture_index;

layout(location = 0) out vec4 fragment_color;

uniform sampler2D u_texture[8];  // FIXME this should be max

void main() {
    const int index = int(v_texture_index);  // Do this just because dynamic texture indexing is UB

    vec4 color;

    switch (int(v_texture_index)) {
        case 0: color = texture(u_texture[0], v_texture_coordinate); break;  // FIXME this should be max
        case 1: color = texture(u_texture[1], v_texture_coordinate); break;
        case 2: color = texture(u_texture[2], v_texture_coordinate); break;
        case 3: color = texture(u_texture[3], v_texture_coordinate); break;
        case 4: color = texture(u_texture[4], v_texture_coordinate); break;
        case 5: color = texture(u_texture[5], v_texture_coordinate); break;
        case 6: color = texture(u_texture[6], v_texture_coordinate); break;
        case 7: color = texture(u_texture[7], v_texture_coordinate); break;
    }

    fragment_color = color;
}
