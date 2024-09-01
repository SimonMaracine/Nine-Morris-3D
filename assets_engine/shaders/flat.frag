#version 430 core

layout(location = 0) out vec4 o_fragment_color;

#include "shaders/common/frag/material/flat.glsl"

void main() {
    o_fragment_color = vec4(u_material.color, 1.0);
}
