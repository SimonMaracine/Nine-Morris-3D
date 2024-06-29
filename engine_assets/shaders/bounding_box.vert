#version 430 core

// TODO remove

layout(location = 0) in vec3 a_position;
layout(location = 1) in float a_entity_id;
layout(location = 2) in mat4 a_model_matrix;

out flat float v_entity_id;

layout(binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

void main() {
    v_entity_id = a_entity_id;

    gl_Position = u_projection_view_matrix * a_model_matrix * vec4(a_position, 1.0);
}
