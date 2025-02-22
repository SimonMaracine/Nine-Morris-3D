#version 430 core

#include "shaders/common/frag/light.glsl"
#include "shaders/common/frag/material/phong_diffuse_normal.glsl"

in vec3 v_fragment_position_tangent_space;
in vec2 v_texture_coordinate;
in vec4 v_fragment_position_light_space;
in vec3 v_view_position_tangent_space;
in vec3 v_light_direction_tangent_space;
in vec3 v_light_position_tangent_space[D_POINT_LIGHTS];

layout(location = 0) out vec4 o_fragment_color;

layout(binding = 2) uniform sampler2D u_shadow_map;
uniform vec3 u_highlight_color;

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[D_POINT_LIGHTS];
};

#include "shaders/common/frag/shadow.glsl"
#include "shaders/common/frag/lighting.glsl"

void main() {
    const vec4 ambient_diffuse = texture(u_material.ambient_diffuse, v_texture_coordinate);
    const vec3 normal = normalize(texture(u_material.normal, v_texture_coordinate).rgb * 2.0 - 1.0);

    const float shadow = calculate_shadow(
        v_fragment_position_light_space,
        normal,
        u_directional_light.direction,
        u_shadow_map
    );

    vec3 color = calculate_directional_light(
        ambient_diffuse.rgb,
        normal,
        v_fragment_position_tangent_space,
        v_view_position_tangent_space,
        v_light_direction_tangent_space,
        shadow
    );

    for (int i = 0; i < D_POINT_LIGHTS; i++) {
        color += calculate_point_light(
            i,
            ambient_diffuse.rgb,
            normal,
            v_fragment_position_tangent_space,
            v_view_position_tangent_space,
            v_light_position_tangent_space[i]
        );
    }

    o_fragment_color = vec4(color + u_highlight_color, ambient_diffuse.a);
}
