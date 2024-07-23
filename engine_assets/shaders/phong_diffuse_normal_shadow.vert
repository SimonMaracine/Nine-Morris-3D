#version 430 core

#include "shaders/common/frag/light.glsl"

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texture_coordinate;
layout(location = 3) in vec3 a_tangent;

out vec3 v_fragment_position_tangent_space;
out vec2 v_texture_coordinate;
out vec4 v_fragment_position_light_space;
out vec3 v_view_position_tangent_space;
out vec3 v_light_direction_tangent_space;
out vec3 v_light_position_tangent_space[D_POINT_LIGHTS];

uniform mat4 u_model_matrix;

layout(shared, binding = 0) uniform ProjectionView {
    mat4 u_projection_view_matrix;
};

layout(shared, binding = 4) uniform LightSpace {
    mat4 u_light_space_matrix;
};

layout(shared, binding = 2) uniform View {
    vec3 u_view_position;
};

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[D_POINT_LIGHTS];
};

#include "shaders/common/vert/tangent_space.glsl"

void main() {
    const mat3 TBN = transpose(calculate_tbn_matrix(u_model_matrix, a_normal, a_tangent));

    v_fragment_position_tangent_space = TBN * vec3(u_model_matrix * vec4(a_position, 1.0));
    v_texture_coordinate = a_texture_coordinate;
    v_fragment_position_light_space = u_light_space_matrix * u_model_matrix * vec4(a_position, 1.0);
    v_view_position_tangent_space = TBN * u_view_position;
    v_light_direction_tangent_space = TBN * u_directional_light.direction;
    for (int i = 0; i < D_POINT_LIGHTS; i++) {
        v_light_position_tangent_space[i] = TBN * u_point_lights[i].position;
    }

    gl_Position = u_projection_view_matrix * u_model_matrix * vec4(a_position, 1.0);
}
