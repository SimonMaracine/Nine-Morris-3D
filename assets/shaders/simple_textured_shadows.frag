#version 430 core

in vec2 v_texture_coordinate;
in vec3 v_normal;
in vec3 v_fragment_position;
in vec4 v_fragment_position_light_space;

layout(location = 0) out vec4 fragment_color;

layout(binding = 1) uniform sampler2D u_shadow_map;

struct Material {
    sampler2D ambient_diffuse;
    vec3 specular;
    float shininess;
};

uniform Material u_material;

struct DirectionalLight_ {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLight_ u_directional_light;
};

struct PointLight_ {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float falloff_linear;
    float falloff_quadratic;
};

const int POINT_LIGHTS = 4;

layout(shared, binding = 3) uniform PointLight {
    PointLight_ u_point_lights[POINT_LIGHTS];
};

layout(shared, binding = 2) uniform ViewPosition {
    vec3 u_view_position;
};

// Lighting calculations are done more efficiently in view space, rather than world space
// This is called Phong shading

#include "shadows.glsl"

vec3 calculate_directional_light() {
    const vec3 color = vec3(texture(u_material.ambient_diffuse, vec2(v_texture_coordinate.x, 1.0 - v_texture_coordinate.y)));

    // Ambient light
    const vec3 ambient_light = color * u_directional_light.ambient;

    // Diffuse light
    const vec3 normal = normalize(v_normal);
    const vec3 light_direction = normalize(-u_directional_light.direction);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    const vec3 diffuse_light = color * u_directional_light.diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(u_view_position - v_fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    const vec3 specular_light = u_material.specular * u_directional_light.specular * specular_strength;

    // All together
    const float shadow = calculate_shadow(v_fragment_position_light_space, normal, u_directional_light.direction, u_shadow_map);
    const vec3 result = ambient_light + (diffuse_light + specular_light) * (1.0 - shadow);

    return result;
}

#include "lighting.glsl"

void main() {
    vec3 color = calculate_directional_light();

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(i);
    }

    fragment_color = vec4(color, 1.0);
}
