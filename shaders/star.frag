#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;
layout(location = 3) in vec3 v_camera_position;
layout(location = 4) in vec3 v_light_position;

uniform sampler2D texture1;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform sampler2D depthmap;

uniform mat4 model;
uniform vec3 camera_position;
uniform float time;
uniform float fInnerRadius;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;
const float MAX = 10000.0;

float snoise(vec3 uv, float res)	// by trisomie21
{
	const vec3 s = vec3(1e0, 1e2, 1e4);
	
	uv *= res;
	
	vec3 uv0 = floor(mod(uv, res))*s;
	vec3 uv1 = floor(mod(uv+vec3(1.), res))*s;
	
	vec3 f = fract(uv); f = f*f*(3.0-2.0*f);
	
	vec4 v = vec4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);
	
	vec4 r = fract(sin(v*1e-3)*1e5);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-3)*1e5);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
}

vec2 rayIntersection(vec3 p, vec3 dir, float radius ) {
	float b = dot( p, dir );
	float c = dot( p, p ) - radius * radius;
	
	float d = b * b - c;
	if ( d < 0.0 ) {
		return vec2( MAX, -MAX );
	}
	d = sqrt( d );
	
	float near = -b - d;
	float far = -b + d;
	
	return vec2(near, far);
}

vec3 getSunColor(vec3 eye_position, vec3 eye_dir, float size) {
	float brightness	= 0.1;
	vec3 orange			= vec3( 0.8, 0.65, 0.3 );
	
	vec2 sphere_intersection = rayIntersection(eye_position, eye_dir, size);

	vec3 p_hit_near = eye_position + eye_dir * sphere_intersection.x; 
	vec3 p_hit_far = eye_position + eye_dir * sphere_intersection.y;
	// 100.0 magic number adjust to what looks right
	float r = dot(p_hit_near-p_hit_far,p_hit_near-p_hit_far)/(fInnerRadius*fInnerRadius);

	//return vec3(r,r,r);
	return vec3( r * ( 0.75 + brightness * 0.3 ) * orange );
}

void main()
{
	//vec3 view_dir = normalize(v_camera_position - v_position);
	vec3 view_dir = normalize(vec3(model*vec4(v_position, 1.0) - vec4(v_camera_position, 1.0)));

	vec3 eye = v_camera_position;
	vec3 l = normalize(v_light_position);

	vec2 e = rayIntersection(eye, view_dir, fInnerRadius);
	float time = time * 0.5;

	
	//vec3 I = inScatter(eye, view_dir, e, l);

	float brightness	= 0.1;
	float radius		= 7.0;
	float invRadius 	= 1.0/radius;
	vec3 noise_coord = vec3(atan( v_tex_coords.x, v_tex_coords.y ), length(v_tex_coords), time * 0.1);

	float n1 = snoise(noise_coord + vec3( 0.0, -time * ( 0.35 + brightness * 0.001 ), time * 0.015 ), 15.0);
	float n2 = snoise(noise_coord + vec3( 0.0, -time * ( 0.15 + brightness * 0.001 ), time * 0.015 ), 45.0);

	vec3 p_hit_near = eye + view_dir * e.x; 
	vec3 p_hit_far = eye + view_dir * e.y; 
	float lp_hit = length(p_hit_near-p_hit_far)/(2.0*fInnerRadius);
	float fade		= pow(1.0-lp_hit, 0.5);//pow( length( 2.0 * 1.0/lp_hit ), 0.5 );
	float fVal1		= 1.0 - fade;
	float fVal2		= 1.0 - fade;
	for( int i=1; i<=7; i++ ){
		float power = pow( 2.0, float(i + 1) );
		fVal1 += ( 0.5 / power ) * snoise( noise_coord + vec3( 0.0, -time, time * 0.2 ), ( power * ( 10.0 ) * ( n1 + 1.0 ) ) );
		fVal2 += ( 0.5 / power ) * snoise( noise_coord + vec3( 0.0, -time, time * 0.2 ), ( power * ( 25.0 ) * ( n1 + 1.0 ) ) );
	}

	float corona		= pow( fVal1 * max( 1.1 - fade, 0.0 ), 2.0 ) * 0.50;
	corona				+= pow( fVal2 * max( 1.1 - fade, 0.0 ), 2.0 ) * 0.50;
	corona				*= 1.2 - n1;

	
	vec2 f = rayIntersection(eye, view_dir, 5.0);
	vec3 f_hit_near = eye + view_dir * f.x;

	//float lp_hit = length(p_hit_far);

	float r = dot(p_hit_near-p_hit_far,p_hit_near-p_hit_far)/100.0;
	float ff = (1.0-sqrt(abs(1.0-r)))/(r) * 0.5;
	

	float starGlow	= min( max( 1.0 - length(p_hit_near-p_hit_far)/10.0 * ( 1.0 - brightness ), 0.0 ), 1.0 );
	vec3 starSphere = vec3(0.0);

	vec3 orange			= vec3( 0.8, 0.65, 0.3 );
	vec3 orangeRed		= vec3( 0.8, 0.35, 0.1 );

	
	//vec3 fragColor = vec3( ff * ( 0.75 + brightness * 0.3 ) * orange ) + starSphere + corona * orange + starGlow * orangeRed;
	vec3 I = vec3(starGlow,starGlow,0.0);

	if( starGlow < r/2.0 ){
		I = vec3(0.0, 1.0, 0.0);
		corona *= pow( starGlow * invRadius, 24.0 );
  		/*vec2 newUv;
 		newUv.x = sp.x*f;
  		newUv.y = sp.y*f;
		newUv += vec2( time, 0.0 );
		
		vec3 texSample 	= texture( iChannel0, newUv ).rgb;
		float uOff		= ( texSample.g * brightness * 4.5 + time );
		vec2 starUV		= newUv + vec2( uOff, 0.0 );*/
		//starSphere		= texture( texture1, starUV ).rgb;
	}
	vec3 fragColor = vec3( ff * ( 0.75 + brightness * 0.3 ) * orange ) + starSphere + corona * orange + starGlow * orangeRed;
	//vec3 fragColor = vec3( ff * ( 0.75 + brightness * 0.3 ) * orange );// + starSphere + corona * orange + starGlow * orangeRed;
	//I = vec3(0.0, ff, 0.0);

	I = vec3( r, r, r);// * orangeRed;

	vec3 sun = getSunColor(v_camera_position, view_dir, fInnerRadius);
	if( r > 7.0 ){
		sun = sun + abs(n1)+abs(n2)*texture( texture1, v_tex_coords ).rgb;
		//I = vec3( r * ( 0.75 + brightness * 0.3 ) * orange );
	}
	

	gl_FragColor = vec4(sun, 1.0);
}