#version 430 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec3 v_normal;
out vec3 v_fragment_position;

uniform mat4 u_model_matrix;

layout(shared, binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

void main() {
    v_normal = mat3(transpose(inverse(u_model_matrix))) * a_normal;  // TODO only uniform scaling
    v_fragment_position = vec3(u_model_matrix * vec4(a_position, 1.0));

    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
