#version 430 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texture_coordinate;
layout(location = 2) in int a_index;

out vec2 v_texture_coordinate;
out flat int v_index;

const int MAX_TEXTS = 32;

uniform mat4 u_model_matrix[MAX_TEXTS];
uniform mat4 u_projection_matrix;

void main() {
    v_texture_coordinate = a_texture_coordinate;
    v_index = a_index;

    gl_Position = u_projection_matrix * u_model_matrix[a_index] * vec4(a_position, 0.0, 1.0);
}
