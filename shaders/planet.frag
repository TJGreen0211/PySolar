#version 450

in vec3 te_position;
in vec3 te_tangent;
in vec3 te_normal;
in vec3 te_test;
in vec2 te_tex_coords;
in vec3 te_camera_position;
in vec3 te_lightPosition;

const vec3 rock_dark = vec3(0.332, 0.332, 0.332);
const vec3 rock_light = vec3(0.5312, 0.5312, 0.5312);
const vec3 snow = vec3(0.7343, 0.7343, 0.6679);
const vec3 grass_light = vec3(0.7851, 0.8203, 0.6054);
const vec3 grass_mountain = vec3(136, 153, 119);
const vec3 grass_mountain_light = vec3(153, 171, 119);
const vec3 grass_plain = vec3(0.0625, 0.6679, 0.332);
const vec3 grass_plain_dark = vec3(0.4023, 0.5742, 0.3476);
const vec3 grass_plain_darkest = vec3(67, 136, 85);
const vec3 beach = vec3(0.8203, 0.7226, 0.5429);
const vec3 barren = vec3(154, 140, 119);
const vec3 forest_light = vec3(0.3359, 0.5976, 0.2656);
const vec3 forest_dark = vec3(0.1992, 0.4648, 0.332);
const vec3 ocean = vec3(0.2617, 0.2617, 0.4765);

uniform sampler2D texture1;

uniform sampler2D wave_dx;
uniform sampler2D wave_dy;
uniform sampler2D wave_dz;


const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;
const vec3 fog_color = vec3(0.7, 0.6, 1.0);

out vec4 frag_color_out;

const vec3 SEA_BASE = vec3(0.0,0.09,0.18);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6)*0.6;

float FogUniform(float dist, float density)
{
    return 1.0 - exp(-dist * density);
}

vec3 getFogFactor(float d, vec3 color)
{
    const float FogMax = 10000.0;
    const float FogMin = 1.0;

	float fog0 = 0.50 * FogUniform(d, 0.0010);

    //if (d>=FogMax) return 0;
    //if (d<=FogMin) return 0;
	return mix(color, fog_color, fog0);
}

vec3 illuminate(float height) {
	// materials
	//#define c_water vec3(.015, .110, .455)
	//#define c_grass vec3(.086, .132, .018)
	//#define c_beach vec3(.153, .172, .121)
	//#define c_rock  vec3(.080, .050, .030)
	//#define c_snow  vec3(.600, .600, .600)

	#define c_water vec3(0.2617, 0.2617, 0.4765)
	#define c_grass vec3(0.4023, 0.5742, 0.3476)
	#define c_beach vec3(0.8203, 0.7226, 0.5429)
	#define c_rock  vec3(0.332, 0.332, 0.332)
	#define c_snow  vec3(0.7343, 0.7343, 0.6679)

	// limits
	#define l_water .12
	#define l_shore .21
	#define l_grass .411
	#define l_rock .651

	float s = smoothstep(.4, 1., height);
	vec3 rock = mix(c_rock, c_snow, smoothstep(1. - .3*s, 1. - .2*s, height));
	vec3 grass = mix(c_grass, rock, smoothstep(l_grass, l_rock, height));
	vec3 shoreline = mix(c_beach, grass, smoothstep(l_shore, l_grass, height));
	vec3 water = mix(c_water / 2., c_water, smoothstep(0., l_water, height));

	//vec3 L = mul(local_xform, normalize(vec3(1, 1, 0)));
	//shoreline *= setup_lights(L, normal);
	//vec3 ocean = setup_lights(L, w_normal) * water;

	vec3 ocean = water;
	
	return mix(ocean, shoreline, smoothstep(l_water, l_shore, height));
}

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
	float current_depth_map_value = texture(texture1, current_tex_coords).r;
	while(current_layer_depth < current_depth_map_value) {
		current_tex_coords -= delta_tex_coords;
		current_depth_map_value = texture(texture1, current_tex_coords).r;
		current_layer_depth += layer_depth;
	}

	vec2 prev_tex_coords = current_tex_coords + delta_tex_coords;
	float after_depth = current_depth_map_value - current_layer_depth;
	float before_depth = texture(texture1, prev_tex_coords).r - current_layer_depth + layer_depth;
	float weight = after_depth / (after_depth - before_depth);
	vec2 final_tex_coords = prev_tex_coords * weight + current_tex_coords * (1.0 - weight);

	return final_tex_coords;
}

vec3 getSeaColor(vec3 p, vec3 n, float diffuse, float specular, vec3 eye, vec3 dist) {  
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.5;
        
    vec3 reflected = vec3(0.3, 0.1, 1.0);    
    vec3 refracted = SEA_BASE + diffuse * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - 0.6) * 0.18 * atten;
    
    //color += vec3(specular(n,l,eye,60.0));
    
    return color;
}

void main()
{
	vec3 color = texture(texture1, te_tex_coords).rgb;
	vec3 view_dir = normalize(te_camera_position - te_position);

	//vec2 parralax_tex_coords = parallax_map(te_tex_coords,  view_dir);
	vec3 normal = normalize(te_test);
	//normal = normalize(normal * 2.0 - 1.0);

	vec3 light_dir = normalize(te_lightPosition - te_position);

	float lambertian = max(dot(light_dir, normal), 0.0);
  	float specular = 0.0;
	if(lambertian > 0.0) {
		// Specular
		vec3 reflect_dir = reflect(-light_dir, normal);
		float spec_angle = max(dot(reflect_dir, view_dir), 0.0);
		specular = pow(spec_angle, 16.0);
	}

	vec3 frag_color = vec3(lambertian*(diffuseColor) + specular*(specColor));

	//color = illuminate(color);

	
	vec3 c = color;


	//vec3 wave_dx_color = texture(wave_dx, te_tex_coords*4.0).rgb;
	//vec3 wave_dy_color = texture(wave_dy, te_tex_coords*4.0).rgb;
	//vec3 wave_dz_color = texture(wave_dz, te_tex_coords*4.0).rgb;

	//vec3 final_wave_color = (wave_dx_color + wave_dy_color + wave_dz_color)/5.0;

	float height = length(color);
	color = illuminate(height);

	//vec3 sea = getSeaColor(final_wave_color, normal, 0.05, specular, view_dir, te_camera_position - te_position); 

	vec3 ambient = 0.05 * color;
	frag_color = (color*(frag_color + ambient));
	vec3 fog = getFogFactor(length(te_camera_position - te_position), frag_color);
	frag_color_out = vec4(frag_color, 1.0);

}