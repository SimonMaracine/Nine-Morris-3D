#version 430 core

layout(location = 0) out float entity_id;

uniform float u_entity_id;

void main() {
    entity_id = u_entity_id;
}
