#version 430 core

in vec2 v_texture_coordinate;
in flat int v_entity_id;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out int entity_id;

uniform sampler2D u_diffuse;

void main() {
    fragment_color = texture(u_diffuse, v_texture_coordinate);
    entity_id = v_entity_id;
}
