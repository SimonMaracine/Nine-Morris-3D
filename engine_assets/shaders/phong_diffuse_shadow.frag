#version 430 core

#include "shaders/common/frag/light.glsl"
#include "shaders/common/frag/material/phong_diffuse.glsl"

in vec3 v_normal;
in vec3 v_fragment_position;
in vec2 v_texture_coordinate;
in vec4 v_fragment_position_light_space;

layout(location = 0) out vec4 o_fragment_color;

layout(binding = 2) uniform sampler2D u_shadow_map;

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[POINT_LIGHTS];
};

layout(shared, binding = 2) uniform View {
    vec3 u_view_position;
};

#include "shaders/common/frag/shadow.glsl"
#include "shaders/common/frag/lighting.glsl"

void main() {
    const vec3 ambient_diffuse = vec3(texture(u_material.ambient_diffuse, v_texture_coordinate));

    const float shadow = calculate_shadow(
        v_fragment_position_light_space,
        v_normal,
        u_directional_light.direction,
        u_shadow_map
    );

    vec3 color = calculate_directional_light(
        ambient_diffuse,
        v_normal,
        v_fragment_position,
        u_view_position,
        u_directional_light.direction,
        shadow
    );

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(
            i,
            ambient_diffuse,
            v_normal,
            v_fragment_position,
            u_view_position,
            u_point_lights[i].position
        );
    }

    o_fragment_color = vec4(color, 1.0);
}
