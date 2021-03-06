#version 450

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec3 camera_position;
uniform vec3 lightPosition;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;

layout(location = 0) out vec4 v_color;
layout(location = 1) out vec2 f_tex_coords;

layout(location = 2) out vec3 fE;
layout(location = 3) out vec3 fN;
layout(location = 4) out vec3 fL;
layout(location = 5) out vec3 fH;
layout(location = 6) out vec3 f_position;
layout(location = 7) out vec3 f_normal;
layout(location = 8) out vec3 f_tangent;


void main()
{
    vec4 ray = normalize(model*vec4(a_position, 1.0) - vec4(camera_position, 1.0));
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec4 lightPos = vec4(lightPosition, 1.0);
    vec3 lightDir = normalize(vec4(a_position, 1.0)*model - lightPos).xyz;

    f_tex_coords = vec2((atan(a_position.x, a_position.y) / 3.1415926 + 1.0) * 0.5,
                        (asin(a_position.z) / 3.1415926 + 0.5));
    v_color = (vec4(a_position, 1.0)+1.0)/2.0;
    fE = -normalize(vec4(a_position, 1.0)*model).xyz;
	fN = normalize(v_color.xyz*normalMatrix);
	fL = -normalize(lightDir);
	fH = -normalize((vec4(a_position, 1.0)*model - lightPos) + ray).xyz;

    f_position = a_position;
    f_normal = a_normal;
    f_tangent = a_tangent;

    gl_Position = vec4(a_position, 1.0)*model*view*projection;
    
}