#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_tex_coords;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec2 v_tex_coords;

void main()
{
    v_position = a_position;
	v_normal = a_normal;
	v_tangent = a_tangent;
	v_tex_coords = a_tex_coords;
}