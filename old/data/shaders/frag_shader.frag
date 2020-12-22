#version 330 core

in vec2 pass_texture_coordinates;

out vec4 final_frag_color;

uniform sampler2D texture;

void main()
{
    final_frag_color = texture2D(texture, pass_texture_coordinates);
}
