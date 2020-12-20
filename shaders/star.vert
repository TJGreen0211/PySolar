#version 450

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec3 camera_position;
uniform vec3 lightPosition;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_tex_coords;


layout(location = 0) out vec3 v_position;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec2 v_tex_coords;
layout(location = 3) out vec3 v_camera_position;
layout(location = 4) out vec3 v_light_position;


void main()
{
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    v_normal = normalize(a_normal*normal_matrix);
    v_tex_coords = a_tex_coords;
    v_position = a_position;//vec3(vec4(a_position, 1.0)*model).xyz;
    v_camera_position = camera_position;
    v_light_position = lightPosition;

    gl_Position = vec4(a_position, 1.0)*model*view*projection;
    
}