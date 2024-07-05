float calculate_shadow(
    vec4 fragment_position_light_space,
    vec3 normal,
    vec3 light_direction,
    in sampler2D shadow_map
) {
    vec3 projection_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w;
    projection_coordinates = projection_coordinates * 0.5 + 0.5;

    const float current_depth = projection_coordinates.z;
    const float bias = max(0.001 * (1.0 - dot(normal, light_direction)), 0.003);

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

    return 1.0 - shadow;
}

float calculate_shadow_sampler2d_shadow(vec4 fragment_position_light_space, in sampler2DShadow shadow_map) {
    const vec4 coordinates = vec4(fragment_position_light_space.xyz * 0.5 + 0.5, fragment_position_light_space.w);
    return textureProj(shadow_map, coordinates);
}
