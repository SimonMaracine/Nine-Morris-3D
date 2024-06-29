#version 430 core

in vec3 v_normal;
in vec2 v_texture_coordinate;
in vec3 v_fragment_position;
in vec4 v_fragment_position_light_space;

layout(location = 0) out vec4 fragment_color;

#include "shaders/common/light.glsl"

layout(binding = 1) uniform sampler2D u_shadow_map;

struct Material {
    sampler2D ambient_diffuse;
    vec3 specular;
    float shininess;
};

uniform Material u_material;

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[POINT_LIGHTS];
};

layout(shared, binding = 2) uniform ViewPosition {
    vec3 u_view_position;
};

#include "shadows.glsl"
#include "lighting.glsl"

void main() {
    const vec3 ambient_diffuse = vec3(texture(u_material.ambient_diffuse, v_texture_coordinate));

    const float shadow = calculate_shadow(v_fragment_position_light_space, normal, u_directional_light.direction, u_shadow_map);

    vec3 color = calculate_directional_light(ambient_diffuse, shadow);

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(i, ambient_diffuse);
    }

    fragment_color = vec4(color, 1.0);
}
