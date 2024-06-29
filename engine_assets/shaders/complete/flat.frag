#version 430 core

layout(location = 0) out vec4 o_fragment_color;

struct Material {
    vec3 color;
};

uniform Material u_material;

void main() {
    o_fragment_color = vec4(u_material.color, 1.0);
}
