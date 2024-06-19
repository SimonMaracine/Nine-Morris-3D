#version 430 core

layout(location = 0) in vec2 a_position;

out vec2 v_texture_coordinate;

void main() {
    v_texture_coordinate = a_position * 0.5 + 0.5;

    gl_Position = vec4(a_position, 0.0, 1.0);
}
