#version 430 core

in vec3 v_color;

layout(location = 0) out vec4 o_fragment_color;

void main() {
    o_fragment_color = vec4(v_color, 1.0);
}
