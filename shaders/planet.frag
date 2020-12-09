#version 450

in vec3 te_position;
in vec3 te_tangent;
in vec3 te_normal;
in vec3 te_test;
in vec2 te_tex_coords;

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

uniform vec3 camera_position;
uniform vec3 lightPosition;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;

vec3 applyFog( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayOri,   // camera position
               in vec3  rayDir )  // camera to point vector
{
    float fogAmount = 0.1*exp(-rayOri.y)*(1.0-exp(-distance*rayDir.y))/rayDir.y;
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
}

float getFogFactor(float d)
{
    const float FogMax = 5000.0;
    const float FogMin = 1.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main()
{
	//vec2 tex_coords = vec2((atan(te_position.x, te_position.z) / (2.0*3.1415926)) + 0.5, -te_position.y * 0.5 + 0.5);
	vec3 wave_dx_color = texture(wave_dx, te_tex_coords*2.0).rgb;
	vec3 wave_dy_color = texture(wave_dy, te_tex_coords*2.0).rgb;
	vec3 wave_dz_color = texture(wave_dz, te_tex_coords*2.0).rgb;
	
	vec3 final_wave_color = (wave_dx_color + wave_dy_color + wave_dz_color)/5.0;

	vec3 color = texture(texture1, te_tex_coords).rgb;
	vec3 view_dir = normalize(camera_position - te_position);
	vec3 normal = normalize(te_normal);

	vec3 light_dir = normalize(lightPosition - te_position);

	float lambertian = max(dot(light_dir, normal), 0.0);
  	float specular = 0.0;
	if(lambertian > 0.0) {
		// Specular
		vec3 reflect_dir = reflect(-light_dir, normal);
		float spec_angle = max(dot(reflect_dir, view_dir), 0.0);
		specular = pow(spec_angle, 8.0);
	}

	//vec3 fog = applyFog(color, length(camera_position - te_position), camera_position, view_dir);
	float fog = getFogFactor(length(camera_position - te_position));

	vec3 frag_color = vec3(lambertian*(diffuseColor) + specular*(specColor));

	//gl_FragColor = vec4(mix(frag_color*color, vec3(1.0, 1.0, 1.0), fog), 1.0);
	//float gamma = 2.2;
	//gl_FragColor = vec4(pow(frag_color+color, vec3(1.0/gamma)), 1.0);
	//gl_FragColor = vec4(frag_color+color, 1.0);
	

	if(length(color) < 0.1) {
		color = ocean;
	}
	if(length(color) < 0.12) {
		color = beach;
	}
	if(length(color) < 0.17) {
		color = grass_light;
	}
	if(length(color) < 0.22) {
		color = grass_plain;
	}
	if(length(color) < 0.3) {
		color = grass_plain_dark;
	}
	if(length(color) < 0.37) {
		color = forest_dark;
	}
	if(length(color) < 0.45) {
		color = forest_light;
	}
	if(length(color) < 0.5) {
		color = rock_light;
	}
	if(length(color) < 0.55) {
		color = rock_dark;
	}
	if(length(color) < 0.6) {
		color = snow;
	}

	//color = normalize(color);
	

	gl_FragColor = vec4(color, 1.0);
}