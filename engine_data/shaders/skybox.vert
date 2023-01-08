#version 430 core

layout(location = 0) in vec3 a_position;

out vec3 v_texture_coordinate;

uniform mat4 u_projection_view_matrix;

void main() {
    v_texture_coordinate = a_position;

    vec4 position = u_projection_view_matrix * vec4(a_position, 1.0);
    position.z = position.w - 0.01;
    gl_Position = position;
}
