#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out vec4 fPosition;
out mat4 m;
out mat4 v;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{	
	fPosition =  vec4(a_position, 1.0);
	fNormal = a_normal;
	m = model;
	v = view;
	gl_Position =  vec4(a_position, 1.0)*model*view*projection;
}