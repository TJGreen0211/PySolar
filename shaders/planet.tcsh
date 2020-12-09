#version 450
#extension GL_ARB_tessellation_shader : enable

in vec3 v_position[];
in vec3 v_normal[];
in vec3 v_tangent[];
in vec2 v_tex_coords[];

layout(vertices = 3) out;
out vec3 tc_position[];
out vec3 tc_normal[];
out vec3 tc_tangent[];
out vec2 tc_tex_coords[];

uniform mat4 model;
uniform vec3 camera_position;

float level(vec3 v0, vec3 v1) {
	//return clamp(distance(v0, v1)/32.0, 2.0, 16.0);
	//float val = log(abs(clamp(length(v0 - v1), 2.0, 16.0) - 16.0));
	float val = log(16.0 - clamp(length(v0 - v1), 2.0, 16.0))/log(1.7);
	return val >= 1.0 ? val : 1.0;
}

void main()
{
	tc_position[gl_InvocationID] = v_position[gl_InvocationID];
	tc_normal[gl_InvocationID] = v_normal[gl_InvocationID];
	tc_tangent[gl_InvocationID] = v_tangent[gl_InvocationID];
	tc_tex_coords[gl_InvocationID] = v_tex_coords[gl_InvocationID];

	//float l = level(tessPos, vec3(camPosition));
	//vec3 tess_position = vec3(vec4(tc_position[gl_InvocationID], 1.0)*model);
	//float l = level(tess_position, vec3(camera_position));

	if(gl_InvocationID == 0) {
		gl_TessLevelInner[0] = 2.0;//l;
	    gl_TessLevelOuter[0] = 4.0;//l-1.0;
	    gl_TessLevelOuter[1] = 4.0;//l-1.0;
	    gl_TessLevelOuter[2] = 4.0;//l-1.0;
	}
}
