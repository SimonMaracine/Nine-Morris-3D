#version 430 core

in vec2 v_texture_coordinate;

layout(location = 0) out vec4 o_fragment_color;

layout(binding = 0) uniform sampler2D u_screen_texture;

const float gamma = 2.2;

void main() {
    const vec4 color = texture(u_screen_texture, v_texture_coordinate);
    o_fragment_color = vec4(pow(color.rgb, vec3(1.0 / gamma)), color.a);
}
