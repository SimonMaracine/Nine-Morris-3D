#version 430 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texture_coordinate;

out vec3 v_normal;
out vec3 v_fragment_position;
out vec2 v_texture_coordinate;

uniform mat4 u_model_matrix;

layout(shared, binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

void main() {
    v_normal = mat3(u_model_matrix) * a_normal;
    v_fragment_position = vec3(u_model_matrix * vec4(a_position, 1.0));
    v_texture_coordinate = a_texture_coordinate;

    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
