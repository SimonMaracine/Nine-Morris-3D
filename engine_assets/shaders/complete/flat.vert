#version 430 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_model_matrix;

layout(shared, binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

void main() {
    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
