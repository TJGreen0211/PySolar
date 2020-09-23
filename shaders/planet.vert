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
layout(location = 3) out vec3 v_camera_position;
layout(location = 4) out vec3 v_light_position;
layout(location = 5) out vec3 v_test;


void main()
{
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    v_normal = normalize(a_normal*normal_matrix);

    vec3 T = normalize(vec3(a_tangent));
	vec3 N = normalize(vec3(a_normal * normal_matrix));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N)); 
    
    v_tex_coords = vec2((atan(a_position.x, a_position.y) / 3.1415926 + 1.0) * 0.5,
                        (asin(a_position.z) / 3.1415926 + 0.5));
    
    v_position = TBN * vec3(vec4(a_position, 1.0)*model).xyz;
    v_camera_position = TBN * camera_position;
    v_light_position = TBN * lightPosition;

    //v_position = vec3(vec4(a_position, 1.0)*model).xyz;
    //v_camera_position = camera_position;
    //v_light_position = lightPosition;

    v_test = N;

    gl_Position = vec4(a_position, 1.0)*model*view*projection;
    
}