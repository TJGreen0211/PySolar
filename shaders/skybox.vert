#version 450

layout(location = 0) in vec3 a_position;
out vec3 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 inverseProjection = inverse(projection);
    mat3 inverseModelview = transpose(mat3(view));
    vec3 unprojected = (inverseProjection * vec4(a_position, 1.0)).xyz;
    v_tex_coords = inverseModelview * unprojected;

    gl_Position = vec4(a_position, 1.0);


	//vec4 pos = vec4(a_position, 1.0)*model*view*projection;
    //v_tex_coords = normalize(a_position.xyz);
//
    //gl_Position = pos.xyww;
}