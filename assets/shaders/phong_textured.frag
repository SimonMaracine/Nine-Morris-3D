#version 430 core

#include "shaders/common/light.glsl"

in vec3 v_normal;
in vec2 v_texture_coordinate;
in vec3 v_fragment_position;

layout(location = 0) out vec4 fragment_color;

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
    // const vec3 ambient_diffuse = vec3(
    //     texture(u_material.ambient_diffuse, vec2(v_texture_coordinate.x, v_texture_coordinate.y))
    // );

    // vec3 color = calculate_directional_light(ambient_diffuse);

    // for (int i = 0; i < POINT_LIGHTS; i++) {
    //     color += calculate_point_light(i, ambient_diffuse);
    // }

    fragment_color = texture(u_material.ambient_diffuse, v_texture_coordinate);//vec4(ambient_diffuse, 1.0);
}
