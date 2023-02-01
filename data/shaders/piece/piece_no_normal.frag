#version 430 core

in vec2 v_texture_coordinate;
in vec3 v_normal;
in vec3 v_fragment_position;
in vec4 v_fragment_position_light_space;
in vec3 v_light_position;
in vec3 v_view_position;

layout(location = 0) out vec4 fragment_color;

uniform sampler2D u_shadow_map;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
    vec3 tint;
};

uniform Material u_material;

layout(binding = 1) uniform Light {
    vec3 u_light_ambient;
    vec3 u_light_diffuse;
    vec3 u_light_specular;
};

float calculate_shadow(
        vec4 fragment_position_light_space,
        vec3 normal,
        vec3 light_direction,
        sampler2D shadow_map) {
    vec3 projection_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w;
    projection_coordinates = projection_coordinates * 0.5 + 0.5;

    const float current_depth = projection_coordinates.z;

    const float bias = max(0.01 * (1.0 - dot(normal, light_direction)), 0.005);

    float shadow = 0.0;
    const vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

    for (int x = -1; x < 2; x++) {
        for (int y = -1; y < 2; y++) {
            const float pcf_depth = texture(shadow_map, projection_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if (projection_coordinates.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

vec3 calculate_light(
        vec3 light_position,
        vec3 light_ambient,
        vec3 light_diffuse,
        vec3 light_specular,
        vec3 view_position,
        vec3 texture_colors,
        vec3 fragment_position,
        vec4 fragment_position_light_space,
        sampler2D shadow_map,
        vec3 normal) {
    // Ambient light
    const vec3 ambient_light = texture_colors * light_ambient;

    // Diffuse light
    const vec3 normalized_normal = normalize(normal);
    const vec3 light_direction = normalize(light_position - fragment_position);

    const float diffuse_strength = max(dot(normalized_normal, light_direction), 0.0);
    const vec3 diffuse_light = diffuse_strength * texture_colors * light_diffuse;

    // Specular light
    const vec3 view_direction = normalize(view_position - fragment_position);
    const vec3 reflect_direction = reflect(-light_direction, normalized_normal);

    const float specular_strength = pow(max(dot(view_direction, reflect_direction), 0.0), u_material.shininess);
    const vec3 specular_light = u_material.specular * specular_strength * light_specular;

    // Calculate shadow and final result
    const float shadow = calculate_shadow(fragment_position_light_space, normalized_normal, light_direction, shadow_map);
    const vec3 result = ambient_light + (diffuse_light + specular_light) * (1.0 - shadow);

    return result;
}

void main() {
    const vec3 texture_colors = vec3(texture(u_material.diffuse, v_texture_coordinate));

    const vec3 total_light = calculate_light(
        v_light_position, u_light_ambient,
        u_light_diffuse, u_light_specular, v_view_position,
        texture_colors, v_fragment_position, v_fragment_position_light_space,
        u_shadow_map, v_normal
    );

    fragment_color = vec4(total_light * u_material.tint, 1.0);;
}
