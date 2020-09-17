#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;

uniform sampler2D texture1;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform vec3 camera_position;
uniform vec3 lightPosition;
uniform mat4 model;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

void main()
{
	vec3 color = texture(texture1, v_tex_coords).rgb;

	vec3 normal = normalize(v_normal);
	vec3 ray = normalize(camera_position-v_position);
	vec3 lightDir = normalize(lightPosition - v_position);

	float lambertian = max(dot(lightDir,normal), 0.0);
  	float specular = 0.0;
	if(lambertian > 0.0) {
		vec3 viewDir = normalize(-v_position);
		//vec3 viewDir = normalize(-v_position - camera_position);

		// Specular
		vec3 reflectDir = reflect(-lightDir, normal);
		float specAngle = max(dot(reflectDir, ray), 0.0);
		specular = pow(specAngle, 8.0);
	}

	gl_FragColor = vec4( lambertian*(diffuseColor*color) + specular*(specColor*color), 1.0);
	//gl_FragColor = vec4(, 1.0);
}