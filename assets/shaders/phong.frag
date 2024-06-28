#version 430 core

in vec3 v_normal;
in vec3 v_fragment_position;

layout(location = 0) out vec4 fragment_color;

#include "shaders/common/light.glsl"

struct Material {
    vec3 ambient_diffuse;
    vec3 specular;
    float shininess;
    // TODO here could also be normal or emission maps
};

uniform Material u_material;

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[POINT_LIGHTS];
};

layout(shared, binding = 2) uniform ViewPosition {  // TODO maybe just do calculations in view space
    vec3 u_view_position;
};

#include "shaders/common/lighting.glsl"

void main() {
    vec3 color = calculate_directional_light(u_material.ambient_diffuse);

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(i, u_material.ambient_diffuse);  // FIXME optimize this
    }

    fragment_color = vec4(color, 1.0);
}
