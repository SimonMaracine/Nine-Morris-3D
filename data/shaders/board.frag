#version 430 core

in vec2 v_texture_coordinate;
in vec3 v_normal;
in flat int v_entity_id;
in vec3 v_fragment_position;
in vec4 v_fragment_position_light_space;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out int entity_id;

uniform vec3 u_view_position;
uniform sampler2D u_shadow_map;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform Material u_material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light u_light;

vec3 calculate_light(Material material, Light light, vec3 texture_colors, float shadow) {
    // Ambient light
    vec3 ambient_light = texture_colors * light.ambient;

    // Diffuse light
    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(light.position - v_fragment_position);

    float diffuse_strength = max(dot(normal, light_direction), 0.0);
    vec3 diffuse_light = diffuse_strength * texture_colors * light.diffuse;

    // Specular light
    vec3 view_direction = normalize(u_view_position - v_fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);

    float specular_strength = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec3 specular_light = material.specular * specular_strength * light.specular;

    vec3 result = ambient_light + (diffuse_light + specular_light) * (1.0 - shadow);

    return result;
}

float calculate_shadow(vec4 fragment_position_light_space) {
    vec3 projection_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w;
    projection_coordinates = projection_coordinates * 0.5 + 0.5;

    float closest_depth = texture(u_shadow_map, projection_coordinates.xy).x;
    float current_depth = projection_coordinates.z;

    float shadow = current_depth > closest_depth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    vec3 texture_colors = vec3(texture(u_material.diffuse, v_texture_coordinate));

    float shadow = calculate_shadow(v_fragment_position_light_space);
    vec3 total_light = calculate_light(u_material, u_light, texture_colors, shadow);

    // Add everything up
    vec4 result_fragment = vec4(total_light, 1.0);

    fragment_color = result_fragment;
    entity_id = v_entity_id;
}
