#version 430 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

layout(shared, binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

void main() {
    v_color = a_color;

    gl_Position = u_projection_view_matrix * vec4(a_position, 1.0);
}
