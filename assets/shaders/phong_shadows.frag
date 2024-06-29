#version 430 core

in vec3 v_normal;
in vec3 v_fragment_position;
in vec4 v_fragment_position_light_space;

layout(location = 0) out vec4 o_fragment_color;

#include "shaders/common/frag/light.glsl"
#include "shaders/common/frag/material/phong.glsl"

layout(binding = 1) uniform sampler2D u_shadow_map;

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[POINT_LIGHTS];
};

layout(shared, binding = 2) uniform ViewPosition {
    vec3 u_view_position;
};

#include "shaders/common/frag/shadow.glsl"
#include "shaders/common/frag/lighting.glsl"

void main() {
    const float shadow = calculate_shadow(v_fragment_position_light_space, v_normal, u_directional_light.direction, u_shadow_map);

    vec3 color = calculate_directional_light(u_material.ambient_diffuse, shadow);

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(i, u_material.ambient_diffuse);
    }

    o_fragment_color = vec4(color, 1.0);
}
