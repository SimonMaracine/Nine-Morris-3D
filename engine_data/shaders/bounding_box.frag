#version 430 core

in flat float v_entity_id;

layout(location = 0) out float entity_id;

void main() {
    entity_id = v_entity_id;
}
