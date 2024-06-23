#version 430 core

in vec2 v_texture_coordinate;
in flat int v_index;

layout(location = 0) out vec4 fragment_color;

layout(binding = 0) uniform sampler2D u_bitmap;

const int MAX_OBJECTS = 10;

uniform vec3 u_color[MAX_OBJECTS];

void main() {
    fragment_color = vec4(u_color[v_index], texture(u_bitmap, v_texture_coordinate).r);
}
