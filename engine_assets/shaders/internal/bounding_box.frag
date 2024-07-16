#version 430 core

// TODO remove

in flat float v_entity_id;

layout(location = 0) out float o_entity_id;

void main() {
    o_entity_id = v_entity_id;
}
