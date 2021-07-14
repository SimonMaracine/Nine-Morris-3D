#version 430 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texture_coordinate;
layout (location = 2) in int a_entity_id;

out vec2 v_texture_coordinate;
out flat int v_entity_id;

uniform mat4 u_model_matrix;
uniform mat4 u_projection_view_matrix;

void main() {
    v_texture_coordinate = a_texture_coordinate;
    v_entity_id = a_entity_id;
    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
