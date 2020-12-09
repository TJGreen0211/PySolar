#version 410

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

layout(location = 0) in vec3 a_position;

void main() {
	//gl_Position = vPosition;
	gl_Position = vec4(a_position, 1.0);//*model*view*projection;
}