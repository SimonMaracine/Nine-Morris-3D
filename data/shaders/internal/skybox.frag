#version 430 core

in vec3 v_texture_coordinate;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out int entity_id;

uniform samplerCube u_skybox;

void main() {
    fragment_color = texture(u_skybox, v_texture_coordinate);
    entity_id = -1;
}
