#version 430 core

in vec3 v_normal;
in vec3 v_fragment_position;
in vec2 v_texture_coordinate;

layout(location = 0) out vec4 o_fragment_color;

#include "shaders/common/light.glsl"

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

#include "shaders/common/lighting.glsl"

void main() {
    const vec3 ambient_diffuse = vec3(texture(u_material.ambient_diffuse, v_texture_coordinate));

    vec3 color = calculate_directional_light(ambient_diffuse);

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(i, ambient_diffuse);
    }

    o_fragment_color = vec4(color, 1.0);
}
