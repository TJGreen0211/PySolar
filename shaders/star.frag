#version 450

//https://www.shadertoy.com/view/4dXGR4
//https://www.shadertoy.com/view/lsf3RH

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;
layout(location = 3) in vec3 v_camera_position;
layout(location = 4) in vec3 v_light_position;

uniform sampler2D texture1;

uniform mat4 model;
uniform vec3 camera_position;
uniform float time;
uniform float fInnerRadius;

uniform sampler2D permTexture;
uniform sampler1D simplexTexture;
uniform sampler2D gradTexture;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;
const float MAX = 10000.0;

#define ONE 0.00390625
#define ONEHALF 0.001953125

out vec4 frag_color;

void simplex( const in vec3 P, out vec3 offset1, out vec3 offset2 )
{
  vec3 offset0;
 
  vec2 isX = step( P.yz, P.xx ); // P.x >= P.y ? 1.0 : 0.0;  P.x >= P.z ? 1.0 : 0.0;
  offset0.x  = isX.x + isX.y;    // Accumulate all P.x >= other channels in offset.x
  offset0.yz = 1.0 - isX;        // Accumulate all P.x <  other channels in offset.yz

  float isY = step( P.z, P.y );  // P.y >= P.z ? 1.0 : 0.0;
  offset0.y += isY;              // Accumulate P.y >= P.z in offset.y
  offset0.z += 1.0 - isY;        // Accumulate P.y <  P.z in offset.z
 
  // offset0 now contains the unique values 0,1,2 in each channel
  // 2 for the channel greater than other channels
  // 1 for the channel that is less than one but greater than another
  // 0 for the channel less than other channels
  // Equality ties are broken in favor of first x, then y
  // (z always loses ties)

  offset2 = clamp( offset0, 0.0, 1.0 );
  // offset2 contains 1 in each channel that was 1 or 2
  offset1 = clamp( offset0-1.0, 0.0, 1.0 );
  // offset1 contains 1 in the single channel that was 1
}

/*
 * 3D simplex noise. Comparable in speed to classic noise, better looking.
 */
float snoise(vec3 P) {

// The skewing and unskewing factors are much simpler for the 3D case
#define F3 0.333333333333
#define G3 0.166666666667

  // Skew the (x,y,z) space to determine which cell of 6 simplices we're in
 	float s = (P.x + P.y + P.z) * F3; // Factor for 3D skewing
  vec3 Pi = floor(P + s);
  float t = (Pi.x + Pi.y + Pi.z) * G3;
  vec3 P0 = Pi - t; // Unskew the cell origin back to (x,y,z) space
  Pi = Pi * ONE + ONEHALF; // Integer part, scaled and offset for texture lookup

  vec3 Pf0 = P - P0;  // The x,y distances from the cell origin

  // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
  // To find out which of the six possible tetrahedra we're in, we need to
  // determine the magnitude ordering of x, y and z components of Pf0.
  vec3 o1;
  vec3 o2;
  simplex(Pf0, o1, o2);

  // Noise contribution from simplex origin
  float perm0 = texture2D(gradTexture, Pi.xy).a;
  vec3  grad0 = texture2D(gradTexture, vec2(perm0, Pi.z)).rgb * 4.0 - 2.0;
  grad0.z = floor(grad0.z); // Remove small variations due to w
  float t0 = 0.6 - dot(Pf0, Pf0);
  float n0;
  if (t0 < 0.0) n0 = 0.0;
  else {
    t0 *= t0;
    n0 = t0 * t0 * dot(grad0, Pf0);
  }

  // Noise contribution from second corner
  vec3 Pf1 = Pf0 - o1 + G3;
  float perm1 = texture2D(gradTexture, Pi.xy + o1.xy*ONE).a;
  vec3  grad1 = texture2D(gradTexture, vec2(perm1, Pi.z + o1.z*ONE)).rgb * 4.0 - 2.0;
  grad1.z = floor(grad1.z); // Remove small variations due to w
  float t1 = 0.6 - dot(Pf1, Pf1);
  float n1;
  if (t1 < 0.0) n1 = 0.0;
  else {
    t1 *= t1;
    n1 = t1 * t1 * dot(grad1, Pf1);
  }
  
  // Noise contribution from third corner
  vec3 Pf2 = Pf0 - o2 + 2.0 * G3;
  float perm2 = texture2D(gradTexture, Pi.xy + o2.xy*ONE).a;
  vec3  grad2 = texture2D(gradTexture, vec2(perm2, Pi.z + o2.z*ONE)).rgb * 4.0 - 2.0;
  grad2.z = floor(grad2.z); // Remove small variations due to w
  float t2 = 0.6 - dot(Pf2, Pf2);
  float n2;
  if (t2 < 0.0) n2 = 0.0;
  else {
    t2 *= t2;
    n2 = t2 * t2 * dot(grad2, Pf2);
  }
  
  // Noise contribution from last corner
  vec3 Pf3 = Pf0 - vec3(1.0-3.0*G3);
  float perm3 = texture2D(gradTexture, Pi.xy + vec2(ONE, ONE)).a;
  vec3  grad3 = texture2D(gradTexture, vec2(perm3, Pi.z + ONE)).rgb * 4.0 - 2.0;
  grad3.z = floor(grad3.z); // Remove small variations due to w
  float t3 = 0.6 - dot(Pf3, Pf3);
  float n3;
  if(t3 < 0.0) n3 = 0.0;
  else {
    t3 *= t3;
    n3 = t3 * t3 * dot(grad3, Pf3);
  }

  // Sum up and scale the result to cover the range [-1,1]
  return 20.0 * (n0 + n1 + n2 + n3);
}

/*
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
	float time = time * 0.5;
	
	vec2 sphere_intersection = rayIntersection(eye_position, eye_dir, size);

	vec3 p_hit_near = eye_position + eye_dir * sphere_intersection.x; 
	vec3 p_hit_far = eye_position + eye_dir * sphere_intersection.y;
	// 100.0 magic number adjust to what looks right
	float r = dot(p_hit_near-p_hit_far,p_hit_near-p_hit_far)/(fInnerRadius*fInnerRadius);

	float angle = atan( v_tex_coords.x, v_tex_coords.y )/6.2832;
	vec3 noise_coord = vec3(angle, length(v_tex_coords), time * 0.1);
	float n1 = snoise(noise_coord + vec3( 0.0, -time * ( 0.35 + brightness * 0.001 ), time * 0.015 ), 15.0);
	float n2 = snoise(noise_coord + vec3( 0.0, -time * ( 0.15 + brightness * 0.001 ), time * 0.015 ), 45.0);
	vec3 sun = vec3( r * ( 0.75 + brightness * 0.3 ) * orange );
	//return vec3(n2,n2,n2);
	if( r > 7.0 ){
		sun = sun + abs(n1)+abs(n2);//*texture( texture1, v_tex_coords ).rgb;
	}

	return sun;
}*/


vec3 rayDirection(vec3 cam) {
	vec4 ray = model*vec4(v_position,1.0) - vec4(cam, 1.0);
	return normalize(vec3(ray));
}

float snoise(vec2 P) {

// Skew and unskew factors are a bit hairy for 2D, so define them as constants
// This is (sqrt(3.0)-1.0)/2.0
#define F2 0.366025403784
// This is (3.0-sqrt(3.0))/6.0
#define G2 0.211324865405

  // Skew the (x,y) space to determine which cell of 2 simplices we're in
 	float s = (P.x + P.y) * F2;   // Hairy factor for 2D skewing
  vec2 Pi = floor(P + s);
  float t = (Pi.x + Pi.y) * G2; // Hairy factor for unskewing
  vec2 P0 = Pi - t; // Unskew the cell origin back to (x,y) space
  Pi = Pi * ONE + ONEHALF; // Integer part, scaled and offset for texture lookup

  vec2 Pf0 = P - P0;  // The x,y distances from the cell origin

  // For the 2D case, the simplex shape is an equilateral triangle.
  // Find out whether we are above or below the x=y diagonal to
  // determine which of the two triangles we're in.
  vec2 o1;
  if(Pf0.x > Pf0.y) o1 = vec2(1.0, 0.0);  // +x, +y traversal order
  else o1 = vec2(0.0, 1.0);               // +y, +x traversal order

  // Noise contribution from simplex origin
  vec2 grad0 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float t0 = 0.5 - dot(Pf0, Pf0);
  float n0;
  if (t0 < 0.0) n0 = 0.0;
  else {
    t0 *= t0;
    n0 = t0 * t0 * dot(grad0, Pf0);
  }

  // Noise contribution from middle corner
  vec2 Pf1 = Pf0 - o1 + G2;
  vec2 grad1 = texture(permTexture, Pi + o1*ONE).rg * 4.0 - 1.0;
  float t1 = 0.5 - dot(Pf1, Pf1);
  float n1;
  if (t1 < 0.0) n1 = 0.0;
  else {
    t1 *= t1;
    n1 = t1 * t1 * dot(grad1, Pf1);
  }

  // Noise contribution from last corner
  vec2 Pf2 = Pf0 - vec2(1.0-2.0*G2);
  vec2 grad2 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float t2 = 0.5 - dot(Pf2, Pf2);
  float n2;
  if(t2 < 0.0) n2 = 0.0;
  else {
    t2 *= t2;
    n2 = t2 * t2 * dot(grad2, Pf2);
  }

  // Sum up and scale the result to cover the range [-1,1]
  return 70.0 * (n0 + n1 + n2);
}

float fbm ( in vec2 _st) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5),
                    -sin(0.5), cos(0.50));
    for (int i = 0; i < 5; ++i) {
        v += a * snoise(_st);
        _st = rot * _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}


void main()
{
	vec3 view_dir = normalize(vec3(model*vec4(v_position, 1.0) - vec4(v_camera_position, 1.0)));

	//vec3 sun = getSunColor(v_camera_position, view_dir, fInnerRadius);

	//float angle = atan( v_tex_coords.x, v_tex_coords.y )/3.2832;

	vec2 st = v_position.xy*10.0;

	vec2 q = vec2(0.);
    q.x = fbm( st + 0.00*time);
    q.y = fbm( st + vec2(1.0));

    vec2 r = vec2(0.);
    r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15*time );
    r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*time);

    float f = fbm(st+r);

	float n = fbm(v_position.xy*10.0);

	frag_color = vec4(f,f,f, 1.0);

	vec3 color = vec3(0.0);

	color = mix(vec3( 0.8, 0.65, 0.3 ),
               vec3( 0.8, 0.35, 0.1 ),
                clamp((f*f)*4.0,0.0,1.0));

    //color = mix(color,
    //            vec3(0,0,0.164706),
    //            clamp(length(q),0.0,1.0));

    //color = mix(color,
    //            vec3(0.666667,1,1),
    //            clamp(length(r.x),0.0,1.0));

    frag_color = vec4((f*f*f+.6*f*f+.5*f)*color,1.)*2.0;
}