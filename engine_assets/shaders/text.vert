#version 430 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texture_coordinate;

out vec2 v_texture_coordinate;

uniform mat4 u_model_matrix;
uniform mat4 u_projection_matrix;

void main() {
    v_texture_coordinate = a_texture_coordinate;

    gl_Position = u_projection_matrix * u_model_matrix * vec4(a_position, 0.0, 1.0);
}
