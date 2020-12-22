#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
//layout(location = 2) in vec3 normal;

out vec2 pass_texture_coordinates;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    pass_texture_coordinates = texture_coordinates;
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
}
