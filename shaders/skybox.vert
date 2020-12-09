#version 450

in vec3 a_position;
out vec2 f_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos = vec4(a_position, 1.0)*model*view;
    f_tex_coords = vec2((atan(a_position.x, a_position.y) / 3.1415926 + 1.0) * 0.5,
                        (asin(a_position.z) / 3.1415926 + 0.5));
    gl_Position = pos.xyww;
}