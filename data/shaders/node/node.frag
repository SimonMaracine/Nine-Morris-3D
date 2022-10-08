#version 430 core

in flat int v_entity_id;

layout(location = 0) out vec4 fragment_color;
layout(location = 1) out int entity_id;

uniform vec4 u_color;

void main() {
    fragment_color = u_color;
    entity_id = v_entity_id;
}
