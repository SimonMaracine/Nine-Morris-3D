#version 430 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texture_coordinate;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out vec2 v_texture_coordinate;
out vec3 v_fragment_position_tangent;
out vec4 v_fragment_position_light_space;
out vec3 v_light_position_tangent;
out vec3 v_view_position_tangent;

uniform mat4 u_model_matrix;

layout(binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

layout(binding = 2) uniform LightView {
    vec3 u_light_position;
    vec3 u_view_position;
};

layout(binding = 3) uniform LightSpace {
    mat4 u_light_space_matrix;
};

mat3 compute_TBN(mat4 model_matrix, vec3 normal, vec3 tangent) {
    const vec3 bitangent = cross(normal, tangent);

    const vec3 T = normalize(vec3(model_matrix * vec4(tangent, 0.0)));
    const vec3 B = normalize(vec3(model_matrix * vec4(bitangent, 0.0)));
    const vec3 N = normalize(vec3(model_matrix * vec4(normal, 0.0)));

    return mat3(T, B, N);
}

void main() {
    v_texture_coordinate = a_texture_coordinate;

    mat3 TBN = compute_TBN(u_model_matrix, a_normal, a_tangent);
    TBN = transpose(TBN);

    const vec3 fragment_position = vec3(u_model_matrix * vec4(a_position, 1.0));

    v_fragment_position_tangent = TBN * fragment_position;
    v_fragment_position_light_space = u_light_space_matrix * vec4(fragment_position, 1.0);
    v_light_position_tangent = TBN * u_light_position;
    v_view_position_tangent = TBN * u_view_position;

    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
