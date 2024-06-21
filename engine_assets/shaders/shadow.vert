#version 430 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_model_matrix;

layout(shared, binding = 4) uniform LightSpace {
    mat4 u_light_space_matrix;
};

void main() {
    gl_Position = u_light_space_matrix * u_model_matrix * vec4(a_position, 1.0);
}
