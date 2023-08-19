#version 430 core

in vec3 v_normal;
in vec3 v_fragment_position;

layout(location = 0) out vec4 fragment_color;

struct Material {
    // These two can also be maps; these represent the color
    vec3 ambient_diffuse;
    vec3 specular;

    float shininess;
    // TODO here could also be normal or emission maps
};

uniform Material u_material;

struct DirectionalLightStruct {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(shared, binding = 1) uniform DirectionalLight {
    DirectionalLightStruct u_directional_light;
};

struct PointLightStruct {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float falloff_linear;
    float falloff_quadratic;
};

const int POINT_LIGHTS = 4;

layout(shared, binding = 3) uniform PointLight {
    PointLightStruct u_point_lights[POINT_LIGHTS];
};

layout(shared, binding = 2) uniform ViewPosition {  // TODO maybe just do calculations in view space
    vec3 u_view_position;
};

// Lighting calculations are done more efficiently in view space, rather than world space
// This is called Phong shading

vec3 calculate_directional_light() {
    // Ambient light
    const vec3 ambient_light = u_material.ambient_diffuse * u_directional_light.ambient;

    // Diffuse light
    const vec3 normal = normalize(v_normal);
    const vec3 light_direction = normalize(-u_directional_light.direction);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    const vec3 diffuse_light = u_material.ambient_diffuse * u_directional_light.diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(u_view_position - v_fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    const vec3 specular_light = u_material.specular * u_directional_light.specular * specular_strength;

    // All together
    const vec3 result = ambient_light + diffuse_light + specular_light;

    return result;
}

vec3 calculate_point_light(int i) {
    const PointLightStruct light = u_point_lights[i];

    // Attenuation
    const float dist = length(light.position - v_fragment_position);
    const float attenuation = 1.0 / (1.0 + light.falloff_linear * dist + light.falloff_quadratic * dist * dist);

    // Ambient light
    const vec3 ambient_light = u_material.ambient_diffuse * light.ambient;

    // Diffuse light
    const vec3 normal = normalize(v_normal);
    const vec3 light_direction = normalize(light.position - v_fragment_position);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    const vec3 diffuse_light = u_material.ambient_diffuse * light.diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(u_view_position - v_fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    const vec3 specular_light = u_material.specular * light.specular * specular_strength;

    // All together
    const vec3 result = (ambient_light + diffuse_light + specular_light) * attenuation;

    return result;
}

void main() {
    vec3 color = calculate_directional_light();

    for (int i = 0; i < POINT_LIGHTS; i++) {
        color += calculate_point_light(i);  // FIXME optimize this
    }

    fragment_color = vec4(color, 1.0);
}
