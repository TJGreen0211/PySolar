#version 450

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec3 camera_position;
uniform vec3 lightPosition;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;

layout(location = 0) out vec3 v_position;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec2 v_tex_coords;


void main()
{
    mat3 normal_matrix = transpose(inverse(mat3(model)));

    v_position = vec3(vec4(a_position, 1.0)*model).xyz;
    v_normal =  a_normal;
    v_tex_coords = vec2((atan(a_position.x, a_position.y) / 3.1415926 + 1.0) * 0.5,
                        (asin(a_position.z) / 3.1415926 + 0.5));
    gl_Position = vec4(a_position, 1.0)*model*view*projection;
    
}