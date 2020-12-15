
#version 450

in vec3 v_tex_coords;
out vec4 frag_color_out;

uniform samplerCube skybox;

void main()
{
    frag_color_out = texture(skybox, v_tex_coords);
}