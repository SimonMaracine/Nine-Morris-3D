#version 430 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in int a_entity_id;

out flat int v_entity_id;

uniform mat4 u_model_matrix;

uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

void main() {
    v_entity_id = a_entity_id;
    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
