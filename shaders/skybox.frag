#version 450

in vec2 f_tex_coords;
out vec4 FragColor;

uniform sampler2D skybox;

void main()
{
    FragColor = texture(skybox, f_tex_coords);
}