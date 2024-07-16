#version 430 core

in vec2 v_texture_coordinate;
in flat int v_texture_index;

layout(location = 0) out vec4 o_fragment_color;

uniform sampler2D u_texture[8];  // TODO this should be max

void main() {
    vec4 color;

    // Do this, because dynamic texture indexing is not right
    switch (v_texture_index) {
        case 0: color = texture(u_texture[0], v_texture_coordinate); break;
        case 1: color = texture(u_texture[1], v_texture_coordinate); break;
        case 2: color = texture(u_texture[2], v_texture_coordinate); break;
        case 3: color = texture(u_texture[3], v_texture_coordinate); break;
        case 4: color = texture(u_texture[4], v_texture_coordinate); break;
        case 5: color = texture(u_texture[5], v_texture_coordinate); break;
        case 6: color = texture(u_texture[6], v_texture_coordinate); break;
        case 7: color = texture(u_texture[7], v_texture_coordinate); break;  // TODO this should be max
    }

    o_fragment_color = color;
}
