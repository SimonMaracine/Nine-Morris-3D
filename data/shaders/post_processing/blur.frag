#version 430 core

in vec2 v_texture_coordinate;

layout(location = 0) out vec4 fragment_color;

uniform sampler2D u_screen_texture;

const float offset = 1.0 / 300.0;

const vec2 offsets[9] = vec2[](
    vec2(-offset, offset),  // top-left
    vec2(0.0, offset),  // top-center
    vec2(offset, offset),  // top-right
    vec2(-offset, 0.0),  // center-left
    vec2(0.0, 0.0),  // center-center
    vec2(offset, 0.0f),  // center-right
    vec2(-offset, -offset),  // bottom-left
    vec2(0.0, -offset),  // bottom-center
    vec2(offset, -offset)  // bottom-right
);

const float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
);

void main() {
    vec3 samples[9];

    for (int i = 0; i < 9; i++) {
        samples[i] = vec3(texture(u_screen_texture, v_texture_coordinate + offsets[i]));
    }

    vec3 result = vec3(0.0);

    for (int i = 0; i < 9; i++) {
        result += samples[i] * kernel[i];
    }

    fragment_color = vec4(result, 1.0);
}
