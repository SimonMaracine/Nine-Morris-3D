// Lighting calculations are done more efficiently in view space, rather than world space
// This is called Phong shading
void calculate_directional_light(vec3 color, out vec3 ambient_light, out vec3 diffuse_light, out vec3 specular_light) {
    // Ambient light
    ambient_light = color * u_directional_light.ambient;

    // Diffuse light
    const vec3 normal = normalize(v_normal);
    const vec3 light_direction = normalize(-u_directional_light.direction);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    diffuse_light = color * u_directional_light.diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(u_view_position - v_fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    specular_light = u_material.specular * u_directional_light.specular * specular_strength;
}

vec3 calculate_directional_light(vec3 color) {
    vec3 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(color, ambient_light, diffuse_light, specular_light);

    return ambient_light + diffuse_light + specular_light;
}

vec3 calculate_directional_light(vec3 color, float shadow) {
    vec3 ambient_light, diffuse_light, specular_light;
    calculate_directional_light(color, ambient_light, diffuse_light, specular_light);

    return ambient_light + (diffuse_light + specular_light) * (1.0 - shadow);
}

vec3 calculate_point_light(int i, vec3 color) {
    const PointLight_ light = u_point_lights[i];

    // Attenuation
    const float dist = length(light.position - v_fragment_position);
    const float attenuation = 1.0 / (1.0 + light.falloff_linear * dist + light.falloff_quadratic * dist * dist);

    // Ambient light
    const vec3 ambient_light = color * light.ambient;

    // Diffuse light
    const vec3 normal = normalize(v_normal);
    const vec3 light_direction = normalize(light.position - v_fragment_position);
    const float diffuse_strength = max(dot(normal, light_direction), 0.0);
    const vec3 diffuse_light = color * light.diffuse * diffuse_strength;

    // Specular light
    const vec3 view_direction = normalize(u_view_position - v_fragment_position);
    const vec3 reflection = reflect(-light_direction, normal);
    const float specular_strength = pow(max(dot(view_direction, reflection), 0.0), u_material.shininess);
    const vec3 specular_light = u_material.specular * light.specular * specular_strength;

    // All together
    return (ambient_light + diffuse_light + specular_light) * attenuation;
}
