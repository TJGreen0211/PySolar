#version 450

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 f_tex_coords;
layout(location = 6) in vec3 f_position;
layout(location = 7) in vec3 f_normal;
layout(location = 8) in vec3 f_tangent;


layout(location = 2) in vec3 fE;
layout(location = 3) in vec3 fN;
layout(location = 4) in vec3 fL;
layout(location = 5) in vec3 fH;
layout(location = 9) in vec3 fTV;
layout(location = 10) in vec3 fTL;

uniform sampler2D texture1;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform vec3 camera_position;
uniform vec3 lightPosition;
uniform mat4 model;

void main()
{
    vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.5, 0.5, 0.5, 1.0);
	float shininess = 10.01;


	mat3 normalMatrix = transpose(inverse(mat3(model)));

	vec3 T = normalize(vec3(vec4(f_tangent, 0.0) * model));
	vec3 N = normalize(vec3(vec4(f_normal, 0.0)) * normalMatrix);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	vec3 v;
	vec3 fragPos = vec3(vec4(f_position,1.0)*model).xyz;
	vec4 lightPos = vec4(lightPosition, 1.0);
	vec3 lightDir = normalize(vec4(f_position,1.0)*model - lightPos).xyz;
	v.x = dot(lightDir, T);
	v.y = dot(lightDir, B);
	v.z = dot(lightDir, N);
	//vec3 fL = -normalize(v);

	vec4 ray = normalize(model*vec4(f_position,1.0) - vec4(camera_position, 1.0));
	vec3 halfVector = vec3((vec4(f_position, 1.0)*model - lightPos) + ray).xyz;
	v.x = dot(halfVector, T);
	v.y = dot(halfVector, B);
	v.z = dot(halfVector, N);
	//vec3 fH = -normalize(v);

	//vec3 fN = vec3(texture(normal_map, f_tex_coords));
	//fN = normalize(N * 2.0 - 1.0);
	
    float Kd = max(dot(fL, fN), 0.0);
	float Ks = pow(max(dot(fN, fH), 0.0), shininess);
	vec4 color = vec4(texture(texture1, f_tex_coords)).rgba;
	vec3 ambient = ambientProduct.xyz * color.rgb;
	vec3 diffuse = Kd * diffuseProduct.xyz * color.rgb;
	vec3 specular = Ks * specularProduct.xyz;
	if(dot(fL, fN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}

    gl_FragColor = vec4(ambient+diffuse+specular, 1.0);
	//gl_FragColor = vec4(normalize(fTV), 1.0);

	////ambient
	//vec3 color = texture(texture1, f_tex_coords).rgb;
	////diffuse
	//vec3 ambient = 0.05 * color;
	//vec3 light_direction = normalize(lightPosition - f_position);
	//vec3 normal = normalize(f_normal);
	//float diff = max(dot(light_direction, normal), 0.0);
	//vec3 diffuse = diff * color;
	////specular
	//vec3 view_direction = normalize(camera_position - f_position);
	////vec3 reflect_direction = reflect(-light_direction, normal);
	//float spec = 0.0;
	//
	//vec3 reflect_direction = reflect(-light_direction, normal);
	//spec = pow(max(dot(view_direction, reflect_direction), 0.0), 8.0);
	//vec3 specular = vec3(0.3) * spec;

	//gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}