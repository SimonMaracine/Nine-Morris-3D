#version 430 core

layout(location = 0) out vec4 o_fragment_color;

uniform vec3 u_color;

void main() {
    o_fragment_color = vec4(u_color, 1.0);
}
