#version 430 core

in vec2 v_texture_coordinate;
in vec3 v_normal;
in flat int v_entity_id;
in vec3 v_fragment_position;

layout (location = 0) out vec4 fragment_color;
layout (location = 1) out int entity_id;

uniform sampler2D u_diffuse;
uniform vec3 u_light_color;
uniform vec3 u_light_position;

void main() {
    // Ambient light
    const float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * u_light_color;

    // Diffuse light
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_light_position - v_fragment_position);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_light_color;

    // Add everything up
    vec4 result_fragment = vec4(ambient + diffuse, 1.0)
            * texture(u_diffuse, v_texture_coordinate);

    fragment_color = result_fragment;
    entity_id = v_entity_id;
}
