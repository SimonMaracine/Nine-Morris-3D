#version 430 core

#include "shaders/common/frag/light.glsl"
#include "shaders/common/frag/material/phong_diffuse.glsl"

in vec3 v_normal;
in vec3 v_fragment_position;
in vec2 v_texture_coordinate;

layout(location = 0) out vec4 o_fragment_color;

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[D_POINT_LIGHTS];
};

layout(shared, binding = 2) uniform View {
    vec3 u_view_position;
};

#include "shaders/common/frag/lighting.glsl"

void main() {
    const vec3 ambient_diffuse = vec3(texture(u_material.ambient_diffuse, v_texture_coordinate));

    vec3 color = calculate_directional_light(
        ambient_diffuse,
        v_normal,
        v_fragment_position,
        u_view_position,
        u_directional_light.direction
    );

    for (int i = 0; i < D_POINT_LIGHTS; i++) {
        color += calculate_point_light(
            i,
            ambient_diffuse
            v_normal,
            v_fragment_position,
            u_view_position,
            u_point_lights[i].position
        );
    }

    o_fragment_color = vec4(color, 1.0);
}
