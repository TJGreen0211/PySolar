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
uniform sampler2D depthmap;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;

vec2 parallax_map(vec2 tex_coords, vec3 view_dir) {
	const float min_layers = 8;
	const float max_layers = 32;
	float num_layers = mix(max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));

	// Calculate layer size
	float layer_depth = 1.0 / num_layers;
	float current_layer_depth = 0.0;
	
	// Texture coordinate shift per layer from vector P
	vec2 P = view_dir.xy / view_dir.z * heightScale;
	vec2 delta_tex_coords = P / num_layers;
	vec2 current_tex_coords = tex_coords;
	float current_depth_map_value = texture(specular_map, current_tex_coords).r;
	while(current_layer_depth < current_depth_map_value) {
		current_tex_coords -= delta_tex_coords;
		current_depth_map_value = texture(specular_map, current_tex_coords).r;
		current_layer_depth += layer_depth;
	}

	vec2 prev_tex_coords = current_tex_coords + delta_tex_coords;
	float after_depth = current_depth_map_value - current_layer_depth;
	float before_depth = texture(specular_map, prev_tex_coords).r - current_layer_depth + layer_depth;
	float weight = after_depth / (after_depth - before_depth);
	vec2 final_tex_coords = prev_tex_coords * weight + current_tex_coords * (1.0 - weight);

	return final_tex_coords;
}

void main()
{
	vec3 view_dir = normalize(v_camera_position - v_position);
	vec2 tex_coords = parallax_map(v_tex_coords,  view_dir);
	if(tex_coords.x > 1.0 || tex_coords.y > 1.0 || tex_coords.x < 0.0 || tex_coords.y < 0.0)
        discard;

	vec3 color = texture(texture1, tex_coords).rgb;

	//vec3 normal = normalize(v_normal);
	vec3 normal = texture(normal_map, tex_coords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

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
	vec3 frag_color = vec3(lambertian*(diffuseColor*color) + specular*(specColor*color));
	gl_FragColor = vec4(frag_color, 1.0);
	//gl_FragColor = vec4(pow(frag_color,vec3(1.0/gamma)), 1.0);
	//gl_FragColor = vec4(color, 1.0);
}