#version 430 core

in vec3 v_texture_coordinate;

layout(location = 0) out vec4 o_fragment_color;

uniform samplerCube u_skybox;

void main() {
    o_fragment_color = texture(u_skybox, v_texture_coordinate);
}
