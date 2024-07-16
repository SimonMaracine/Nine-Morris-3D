#version 430 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_model_matrix;

layout(binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

uniform float u_outline_thickness;

void main() {
    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position + a_normal * u_outline_thickness, 1.0);
}
