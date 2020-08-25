#version 450

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec3 camera_position;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

layout(location = 0) out vec4 v_color;

layout(location = 1) out vec3 fE;
layout(location = 2) out vec3 fN;
layout(location = 3) out vec3 fL;
layout(location = 4) out vec3 fH;

void main()
{
    vec4 ray = normalize(model*vec4(a_position, 1.0) - vec4(0.0, 5.0, 10.0, 1.0));
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec4 lightPos = vec4(10.0, 5.0, -4.0, 1.0);
    vec3 lightDir = normalize(vec4(a_position, 1.0)*model - lightPos).xyz;

    v_color = (vec4(a_position, 1.0)+1.0)/2.0;
    fE = -normalize(vec4(a_position, 1.0)*model).xyz;
	fN = normalize(v_color.xyz*normalMatrix);
	fL = -normalize(lightDir);
	fH = -normalize((vec4(a_position, 1.0)*model - lightPos) + ray).xyz;

    gl_Position = vec4(a_position, 1.0)*model*view*projection;
    
}