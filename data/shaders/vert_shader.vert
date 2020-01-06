#version 330 core

layout(location = 0) in vec3 position;

//uniform mat4 modelMatrix;
//uniform mat4 modeView;
//uniform mat4 modelProjection;

void main()
{
    gl_Position = vec4(position, 1.0);
}
