#version 410

const float tau = 2.0 * 3.1415926535897;


out vec4 frag_color;

uniform float systemTime;
uniform float width;
uniform float height;
uniform float terrain_scale;

uniform float e0;
uniform float e1;
uniform float e2;
uniform float e3;
uniform float e4;
uniform float e5;


uniform int reverse;
uniform int order_x;
uniform int order_y;
uniform int order_z;

uniform int animated;
uniform sampler2D permTexture;
uniform sampler1D simplexTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

/*
 * Efficient simplex indexing functions by Bill Licea-Kane, ATI. Thanks!
 * (This was originally implemented as a 1D texture lookup. Nice to avoid that.)
 */
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

mat2 m2 = mat2( 0.80,  0.60, -0.60,  0.80 );

float terrain( in vec2 p)
{
    float rz = 0.;
    float z = 1.;
	vec2  d = vec2(0.0);
    float scl = 2.95;
    float zscl = -.4;
    float zz = 5.;
    for( int i=0; i<5; i++ )
    {
        float n = snoise(vec3(p.x, p.y, 1.0));
        d += pow(abs(n),zz);
        d -= smoothstep(-.5,1.5,n);
        zz -= 1.;
        rz += z*n/(dot(d,d)+.85);
        z *= zscl;
        zscl *= .8;
        p = m2*p*scl;
    }
    
    rz /= smoothstep(1.5,-.5,rz)+.75;
    return rz;
}

const mat2 m = mat2(0.8,-0.6,0.6,0.8);

float terrain_fbm(vec2 p ) {
    float a = 0.0;
    float b = 1.0;
    float arr[3] = float[3](p.x, p.y, 1.0);
    vec2  d = vec2(0.0);
    for( int i=0; i<15; i++ )
    {
        float f = snoise(vec3(p.x, p.y, 1.0));
        vec3 n=vec3(f, f, f);
        d +=n.yz;
        a +=b*n.x/(1.0+dot(d,d));
        b *=0.5;
        p=m*p*2.0;
    }
    return a;
  }


void main() {
	//float scale = terrain_scale;
  float scale = 2.71;

  //n = (
  //         e0 * snoise(vec3( 1.0 * nx,  1.0 * ny, 0.0))
  //       + 0.50 * snoise(vec3( 2.0 * nx,  2.0 * ny, 0.0))
  //       + 0.25 * snoise(vec3( 4.0 * nx,  4.0 * ny, 0.0))
  //       + 0.13 * snoise(vec3( 8.0 * nx,  8.0 * ny, 0.0))
  //       + 0.06 * snoise(vec3(16.0 * nx, 16.0 * ny, 0.0))
  //       + 0.03 * snoise(vec3(32.0 * nx, 32.0 * ny, 0.0)));
  //n /= (1.00+0.50+0.25+0.13+0.06+0.03);

  float nx1 = gl_FragCoord.x/width * scale;
	float ny1 = gl_FragCoord.y/height * scale;
  float n = terrain_fbm(vec2(nx1, ny1));
  //n = pow(n, 1.7);

	frag_color = vec4(vec3(n), 1.0);

}
