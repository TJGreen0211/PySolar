#version 450

//https://www.shadertoy.com/view/Ms2SD1

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;
layout(location = 3) in vec3 v_camera_position;
layout(location = 4) in vec3 v_light_position;
layout(location = 5) in vec3 v_test;

uniform sampler2D texture1;
uniform sampler2D wave_dx;
uniform sampler2D wave_dy;
uniform sampler2D wave_dz;
uniform sampler2D specular_map;
uniform sampler2D depthmap;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;

out vec4 frag_color_out;

const int NUM_STEPS = 3;
const float PI	 	= 3.141592;
const float EPSILON	= 1e-3;
#define EPSILON_NRM (0.1 / iResolution.x)
#define AA

const int ITER_GEOMETRY = 3;
const int ITER_FRAGMENT = 5;
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.0,0.09,0.18);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6)*0.6;
#define SEA_TIME (1.0 + iTime * SEA_SPEED)
const mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

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

float specular(vec3 n, vec3 l, vec3 e, float s) {    
    float nrm = (s + 8.0) / (3.14159 * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

vec3 getSkyColor(vec3 e) {
    e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
    return vec3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
}

float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.5;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    //color += vec3(specular(n,l,eye,60.0));
    
    return color;
}

void main()
{
	vec3 view_dir = normalize(v_camera_position - v_position);
	vec2 tex_coords = parallax_map(v_tex_coords,  view_dir);
	if(tex_coords.x > 1.0 || tex_coords.y > 1.0 || tex_coords.x < 0.0 || tex_coords.y < 0.0)
        discard;

	vec3 color = texture(texture1, v_tex_coords).rgb;

	vec3 wave_dx_color = texture(wave_dx, v_tex_coords*4.0).rgb;
	vec3 wave_dy_color = texture(wave_dy, v_tex_coords*4.0).rgb;
	vec3 wave_dz_color = texture(wave_dz, v_tex_coords*4.0).rgb;

	vec3 final_wave_color = (wave_dx_color + wave_dy_color + wave_dz_color)/5.0;

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
		specular = pow(specAngle, 16.0);
	}

	//color += vec3(specular(n,l,eye,60.0));
	//float s = specular(normal, light_dir, view_dir, 60.0);

	float gamma = 2.2;
    //FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
	vec3 frag_color = vec3(lambertian*(diffuseColor) + specular*(specColor));

	//vec3 sea = getSeaColor(final_wave_color, normal, light_dir, view_dir, v_camera_position - v_position); 
	frag_color_out = vec4(frag_color, 1.0);
	//gl_FragColor = vec4(pow(frag_color,vec3(1.0/gamma)), 1.0);
	//gl_FragColor = vec4(color, 1.0);
}