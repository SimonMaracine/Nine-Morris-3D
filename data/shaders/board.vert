#version 430 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texture_coordinate;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;

out vec2 v_texture_coordinate;
out vec3 v_fragment_position_tangent;
out vec4 v_fragment_position_light_space;
out vec3 v_light_position_tangent;
out vec3 v_view_position_tangent;

uniform mat4 u_model_matrix;

layout (binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

layout (binding = 2) uniform LightViewPosition {
    vec3 u_light_position;
    vec3 u_view_position;
};

layout (binding = 3) uniform LightSpace {
    mat4 u_light_space_matrix;
};

void main() {
    v_texture_coordinate = a_texture_coordinate;

    vec3 bitangent = cross(a_normal, a_tangent);

    vec3 T = normalize(vec3(u_model_matrix * vec4(a_tangent, 0.0)));
    vec3 B = normalize(vec3(u_model_matrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(u_model_matrix * vec4(a_normal, 0.0)));
    mat3 TBN = mat3(T, B, N);
    TBN = transpose(TBN);

    vec3 v_fragment_position = vec3(u_model_matrix * vec4(a_position, 1.0));

    v_fragment_position_tangent = TBN * v_fragment_position;
    v_fragment_position_light_space = u_light_space_matrix * vec4(v_fragment_position, 1.0);
    v_light_position_tangent = TBN * u_light_position;
    v_view_position_tangent = TBN * u_view_position;

    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
