#version 450

//https://www.shadertoy.com/view/Ms2SD1

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;
layout(location = 3) in vec3 v_camera_position;
layout(location = 4) in vec3 v_light_position;
layout(location = 5) in vec3 v_test;

uniform sampler2D texture1;
uniform sampler2D specular_map;
uniform sampler2D depthmap;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;


void main()
{
	vec3 view_dir = normalize(v_camera_position - v_position);

	vec3 color = texture(texture1, v_tex_coords).rgb;

	vec3 normal = normalize(v_normal);
	//vec3 normal = texture(normal_map, tex_coords).rgb;
	//normal = normalize(normal * 2.0 - 1.0);

	//vec3 v_light_position = vec3(0.0, 0.0, 0.0);
	vec3 light_dir = normalize(v_light_position - v_position);

	float lambertian = max(dot(light_dir, normal), 0.0);
  	float specular = 0.0;
	if(lambertian > 0.0) {
		// Specular
		vec3 reflectDir = reflect(-light_dir, normal);
		float specAngle = max(dot(reflectDir, view_dir), 0.0);
		specular = pow(specAngle, 8.0);
	}

	float gamma = 2.2;
    //FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
	vec3 frag_color = vec3(lambertian*(diffuseColor) + specular*(specColor));

	gl_FragColor = vec4(frag_color, 1.0);
	//gl_FragColor = vec4(pow(frag_color,vec3(1.0/gamma)), 1.0);
	//gl_FragColor = vec4(color, 1.0);
}