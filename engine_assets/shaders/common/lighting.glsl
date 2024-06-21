vec3 calculate_point_light(int i) {
    const PointLight_ light = u_point_lights[i];
    const vec3 color = vec3(texture(u_material.ambient_diffuse, vec2(v_texture_coordinate.x, 1.0 - v_texture_coordinate.y)));

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
    const vec3 result = (ambient_light + diffuse_light + specular_light) * attenuation;

    return result;
}
