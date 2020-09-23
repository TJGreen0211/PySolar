#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;
layout(location = 3) in vec3 v_camera_position;
layout(location = 4) in vec3 v_light_position;
layout(location = 5) in vec3 v_test;

uniform sampler2D texture1;
uniform sampler2D normal_map;
uniform sampler2D specular_map;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

void main()
{
	vec3 color = texture(texture1, v_tex_coords).rgb;

	//vec3 normal = normalize(v_normal);
	vec3 normal = texture(normal_map, v_tex_coords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 v_light_position = vec3(0.0, 0.0, 0.0);
	vec3 light_dir = normalize(v_light_position - v_position);

	float lambertian = max(dot(light_dir, normal), 0.0);
  	float specular = 0.0;
	if(lambertian > 0.0) {
		vec3 view_dir = normalize(v_camera_position - v_position);

		// Specular
		vec3 reflectDir = reflect(-light_dir, normal);
		float specAngle = max(dot(reflectDir, view_dir), 0.0);
		specular = pow(specAngle, 8.0);
	}

	gl_FragColor = vec4( lambertian*(diffuseColor*color) + specular*(specColor*color), 1.0);
}