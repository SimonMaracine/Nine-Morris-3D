// Lighting calculations are done more efficiently in view space, rather than world space
// This is called Phong shading
void calculate_directional_light(
    vec4 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction,
    out vec4 ambient_light,
    out vec4 diffuse_light,
    out vec4 specular_light
) {
    // Ambient light
    ambient_light = color * vec4(u_directional_light.ambient, 1.0);

    // Diffuse light
    normal = normalize(normal);
    light_direction = normalize(-light_direction);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    diffuse_light = color * vec4(u_directional_light.diffuse, 1.0) * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(view_position - fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    specular_light = vec4(u_material.specular, 0.0) * vec4(u_directional_light.specular, 0.0) * specular_strength;
}

vec4 calculate_directional_light(
    vec4 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction
) {
    vec4 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(
        color,
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
    vec3 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction
) {
    vec4 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(
        vec4(color, 1.0),
        normal,
        fragment_position,
        view_position,
        light_direction,
        ambient_light,
        diffuse_light,
        specular_light
    );

    return vec3(ambient_light + diffuse_light + specular_light);
}

vec4 calculate_directional_light(
    vec4 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction,
    float shadow
) {
    vec4 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(
        color,
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

vec3 calculate_directional_light(
    vec3 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_direction,
    float shadow
) {
    vec4 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(
        vec4(color, 1.0),
        normal,
        fragment_position,
        view_position,
        light_direction,
        ambient_light,
        diffuse_light,
        specular_light
    );

    return vec3(ambient_light + (diffuse_light + specular_light) * shadow);
}

void calculate_point_light(
    int i,
    vec4 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_position,
    out vec4 ambient_light,
    out vec4 diffuse_light,
    out vec4 specular_light
) {
    const PointLight_ light = u_point_lights[i];

    // Attenuation
    const float dist = length(light_position - fragment_position);
    const float attenuation = 1.0 / (1.0 + light.falloff_linear * dist + light.falloff_quadratic * dist * dist);

    // Ambient light
    ambient_light = color * vec4(light.ambient, 1.0) * attenuation;

    // Diffuse light
    normal = normalize(normal);
    const vec3 light_direction = normalize(light_position - fragment_position);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    diffuse_light = color * vec4(light.diffuse, 1.0) * diffuse_strength * attenuation;

    // Specular light
    const vec3 view_direction = normalize(view_position - fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    specular_light = vec4(u_material.specular, 0.0) * vec4(light.specular, 0.0) * specular_strength * attenuation;
}

vec4 calculate_point_light(
    int i,
    vec4 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_position
) {
    const PointLight_ light = u_point_lights[i];

    vec4 ambient_light, diffuse_light, specular_light;
    calculate_point_light(
        i,
        color,
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

vec3 calculate_point_light(
    int i,
    vec3 color,
    vec3 normal,
    vec3 fragment_position,
    vec3 view_position,
    vec3 light_position
) {
    const PointLight_ light = u_point_lights[i];

    vec4 ambient_light, diffuse_light, specular_light;
    calculate_point_light(
        i,
        vec4(color, 1.0),
        normal,
        fragment_position,
        view_position,
        light_position,
        ambient_light,
        diffuse_light,
        specular_light
    );

    return vec3(ambient_light + diffuse_light + specular_light);
}
