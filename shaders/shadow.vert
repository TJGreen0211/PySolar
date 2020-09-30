#version 450

layout(location = 0) in vec3 a_position;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(a_position, 1.0)*model*view*projection;
}
