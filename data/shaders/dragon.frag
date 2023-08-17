#version 430 core

in vec3 v_normal;
in vec3 v_fragment_position;

layout(location = 0) out vec4 fragment_color;

struct Material {
    // These two can also be maps; these represent the color
    vec3 ambient_diffuse;
    vec3 specular;

    float shininess;
    // TODO here could also be normals
};

uniform Material u_material;

layout(shared, binding = 1) uniform Light {
    vec3 u_light_position;
    vec3 u_light_ambient;
    vec3 u_light_diffuse;
    // Specular component omitted
};

layout(shared, binding = 2) uniform ViewPosition {  // TODO maybe just do calculations in view space
    vec3 u_view_position;
};

// Lighting calculations are done more efficiently in view space, rather than world space
// This is called Phong shading

vec3 calculate_light() {
    // Ambient light
    const vec3 ambient_light = u_material.ambient_diffuse * u_light_ambient;

    // Diffuse light
    const vec3 normal = normalize(v_normal);
    const vec3 light_direction = normalize(u_light_position - v_fragment_position);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    const vec3 diffuse_light = u_material.ambient_diffuse * u_light_diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(u_view_position - v_fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    const vec3 specular_light = u_material.specular * specular_strength;

    // All together
    const vec3 result = ambient_light + diffuse_light + specular_light;

    return result;
}

void main() {
    const vec3 color = calculate_light();

    fragment_color = vec4(color, 1.0);
}
