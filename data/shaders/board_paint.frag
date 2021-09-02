#version 430 core

in vec2 v_texture_coordinate;
in vec3 v_normal;
in vec3 v_fragment_position;

out vec4 fragment_color;

uniform vec3 u_view_position;

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

vec4 calculate_light(Material material, Light light, vec4 texture_colors) {
    // Ambient light
    vec4 ambient_light = texture_colors * vec4(light.ambient, 1.0);

    // Diffuse light
    vec3 normal = normalize(v_normal);
    vec3 light_direction = normalize(light.position - v_fragment_position);

    float diffuse_strength = max(dot(normal, light_direction), 0.0);
    vec4 diffuse_light = diffuse_strength * texture_colors * vec4(light.diffuse, 1.0);

    // Specular light
    vec3 view_direction = normalize(u_view_position - v_fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);

    float specular_strength = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    vec4 specular_light = vec4(material.specular, 0.0) * specular_strength * vec4(light.specular, 0.0);

    vec4 result = ambient_light + diffuse_light + specular_light;

    return result;
}

void main() {
    vec4 texture_colors = texture(u_material.diffuse, v_texture_coordinate);

    vec4 total_light = calculate_light(u_material, u_light, texture_colors);

    fragment_color = total_light;
}
