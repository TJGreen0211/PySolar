#version 450

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_tex_coords;

out vec3 v_color;
out vec2 v_tex_coords;

void main()
{
    v_color = a_position;
    v_tex_coords = a_tex_coords;
    gl_Position = vec4(a_position, 1.0);//*model*view*projection;
}

//#version 450
//
//layout (location = 0) in vec2 aPos;
//layout (location = 1) in vec2 aTexCoords;
//
//out vec2 tex_coords;
//
//void main() 
//{
//    tex_coords = aTexCoords;
//    gl_position = vec4(aPos.x, aPos.y, 0.0, 1.0);
//}
