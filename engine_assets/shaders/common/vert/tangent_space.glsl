mat3 calculate_tbn_matrix(mat4 model_matrix, vec3 normal, vec3 tangent) {
    const vec3 bitangent = cross(normal, tangent);

    const vec3 T = normalize(vec3(model_matrix * vec4(tangent, 0.0)));
    const vec3 B = normalize(vec3(model_matrix * vec4(bitangent, 0.0)));
    const vec3 N = normalize(vec3(model_matrix * vec4(normal, 0.0)));

    return mat3(T, B, N);
}
