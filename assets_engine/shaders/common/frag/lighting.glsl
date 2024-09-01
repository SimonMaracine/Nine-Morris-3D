// Lighting calculations are done more efficiently in view space, rather than world space
// This is called Phong shading
void calculate_directional_light(
    vec3 ambient_diffuse,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction,
    out vec3 ambient_light,
    out vec3 diffuse_light,
    out vec3 specular_light
) {
    normal = normalize(normal);

    // Ambient light
    ambient_light = ambient_diffuse * u_directional_light.ambient;

    // Diffuse light
    light_direction = normalize(-light_direction);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    diffuse_light = ambient_diffuse * u_directional_light.diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(view_position - fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    specular_light = u_material.specular * u_directional_light.specular * specular_strength;
}

vec3 calculate_directional_light(
    vec3 ambient_diffuse,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction
) {
    vec3 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(
        ambient_diffuse,
        normal,
        fragment_position,
        view_position,
        light_direction,
        ambient_light,
        diffuse_light,
        specular_light
    );

    return ambient_light + diffuse_light + specular_light;
}

vec3 calculate_directional_light(
    vec3 ambient_diffuse,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction,
    float shadow
) {
    vec3 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(
        ambient_diffuse,
        normal,
        fragment_position,
        view_position,
        light_direction,
        ambient_light,
        diffuse_light,
        specular_light
    );

    return ambient_light + (diffuse_light + specular_light) * shadow;
}

void calculate_point_light(
    int i,
    vec3 ambient_diffuse,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_position,
    out vec3 ambient_light,
    out vec3 diffuse_light,
    out vec3 specular_light
) {
    const PointLight_ light = u_point_lights[i];
    normal = normalize(normal);

    // Attenuation
    const float dist = length(light_position - fragment_position);
    const float attenuation = 1.0 / (1.0 + light.falloff_linear * dist + light.falloff_quadratic * dist * dist);

    // Ambient light
    ambient_light = ambient_diffuse * light.ambient * attenuation;

    // Diffuse light
    const vec3 light_direction = normalize(light_position - fragment_position);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    diffuse_light = ambient_diffuse * light.diffuse * diffuse_strength * attenuation;

    // Specular light
    const vec3 view_direction = normalize(view_position - fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    specular_light = u_material.specular * light.specular * specular_strength * attenuation;
}

vec3 calculate_point_light(
    int i,
    vec3 ambient_diffuse,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_position
) {
    const PointLight_ light = u_point_lights[i];

    vec3 ambient_light, diffuse_light, specular_light;
    calculate_point_light(
        i,
        ambient_diffuse,
        normal,
        fragment_position,
        view_position,
        light_position,
        ambient_light,
        diffuse_light,
        specular_light
    );

    return ambient_light + diffuse_light + specular_light;
}
