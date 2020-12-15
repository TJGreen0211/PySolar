#version 450

#define STEP_SIZE 0.05
#define MAX_STEPS 512
//uniform sampler2D texture1;
//uniform sampler2D texture2;
//uniform sampler2D texture3;

in vec3 v_color;
in vec2 v_tex_coords;

out vec4 frag_color;

uniform float time;
uniform float screen_width;
uniform float screen_height;
uniform int draw_nebula;

const vec3 _Absorption = vec3(0.9,0.7,0.5);
const vec3 _Emission = vec3(0.3,0.15,0.0);
const vec3 _Light = vec3(1.0,1.0,1.0);
const vec3 _LightPos = vec3(0.0, 0.0, 0.0);
const float _LightIntensity = 0.2;
const float _Density = 9.0;
const float _Radius = 1.0;
const float _Mie = 0.6;
const float _G = 0.2;

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


//Ray-sphere intersection
bool raycastSphere(vec3 ro, vec3 rd, out vec3 p0, out vec3 p1, vec3 center, float r)
{
    float A = 1.0; //dot(rd, rd);
    float B = 2.0 * (rd.x * (ro.x - center.x) + rd.y * (ro.y - center.y) + rd.z * (ro.z - center.z));
    float C = dot(ro - center, ro - center) - (r * r);

    float D = B * B - 4.0 * A * C;
    if (D < 0.0)
    {
        return false;
    }
    else
    {
        float t0 = (-B - D)/(2.0 * A);
        float t1 = (-B + D)/(2.0 * A);
        p0 = ro + rd * t0;
        p1 = ro + rd * t1;
        return true;
    }
}

vec3 rotateY(vec3 p, float t)
{
    float cosTheta = cos(t);
    float sinTheta = sin(t);
    mat3 rot = mat3(cosTheta, 0.0, sinTheta,
        			0.0, 1.0, 0.0,
    			    -sinTheta, 0.0, cosTheta);
    
    return rot * p;
}


//iq's gradient noise
vec3 hash( vec3 p )
{
	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
			  dot(p,vec3(269.5,183.3,246.1)),
			  dot(p,vec3(113.5,271.9,124.6)));

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

const mat2 m2 = mat2(0.8,-0.6,0.6,0.8);

float fbm( in vec2 p ){
    float f = 0.0;
    f += 0.5000*snoise( vec3(0.0, p), 1.0 ); p = m2*p*2.02;
    f += 0.2500*snoise( vec3(0.0, p), 1.0 ); p = m2*p*2.03;
    f += 0.1250*snoise( vec3(0.0, p), 1.0 ); p = m2*p*2.01;
    f += 0.0625*snoise( vec3(0.0, p), 1.0 );

    return f/0.9375;
}

float noise( in vec3 p )
{
    vec3 i = floor( p );
    vec3 f = fract( p );
	
	vec3 u = f*f*(3.0-2.0*f);

    return mix( mix( mix( dot( hash( i + vec3(0.0,0.0,0.0) ), f - vec3(0.0,0.0,0.0) ), 
                          dot( hash( i + vec3(1.0,0.0,0.0) ), f - vec3(1.0,0.0,0.0) ), u.x),
                     mix( dot( hash( i + vec3(0.0,1.0,0.0) ), f - vec3(0.0,1.0,0.0) ), 
                          dot( hash( i + vec3(1.0,1.0,0.0) ), f - vec3(1.0,1.0,0.0) ), u.x), u.y),
                mix( mix( dot( hash( i + vec3(0.0,0.0,1.0) ), f - vec3(0.0,0.0,1.0) ), 
                          dot( hash( i + vec3(1.0,0.0,1.0) ), f - vec3(1.0,0.0,1.0) ), u.x),
                     mix( dot( hash( i + vec3(0.0,1.0,1.0) ), f - vec3(0.0,1.0,1.0) ), 
                          dot( hash( i + vec3(1.0,1.0,1.0) ), f - vec3(1.0,1.0,1.0) ), u.x), u.y), u.z );
}

//Cornette-Shanks phase function
float phase(float mu, float g)
{
	float g2 = g * g;
	return (3.0 * (1.0 - g2) * (1.0 + mu * mu)) / (2.0 * (2.0 + g2) * pow(1.0 + g2 - 2.0 * g * mu, 1.5));
}

//Otavio Good's fast spiral noise from https://www.shadertoy.com/view/ld2SzK
const float nudge = 0.739513;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p)
{
    float n = 0.0;	// noise amount
    float iter = 1.0;
    for (int i = 0; i < 6; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;	// abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += vec2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}

float sampleVolume(vec3 pos)
{
    float rr = dot(pos,pos);
    rr = sqrt(rr);
    float f = exp(-rr);
    float p = f * _Density;
    
    if (p <= 0.0)
        return p;
    
    p += SpiralNoiseC(512.0 + pos * 8.0) * 0.75;
    pos = rotateY(pos, pos.y * SpiralNoiseC(pos * 4.0)* 2.0);
    p += SpiralNoiseC(200.0 + pos * 3.0) * 1.5;
    p *= rr/_Radius;
        
    p = max(0.0,p);
                
    return p;
}

vec4 raymarch(vec3 pos, vec3 dir, float ds, int s)
{
    vec4 result = vec4(0.,0.0,0.0,1.0);
    int steps = min(s, MAX_STEPS);
    for (int i = 0; i < steps; i++)
    {
        float p = sampleVolume(pos);
        if (p > 0.0)
        {
            vec3 r = _LightPos - pos;
            float atten = _LightIntensity/dot(r, r);
            vec3 ext = max(vec3(0.000001), (_Absorption * p) + vec3(_Mie * p));
            vec3 trans = exp(-ext * ds);
            vec3 lightDir = normalize(r);
            float mu = dot(lightDir, dir);
            float phase = phase(mu, _G);
            vec3 lum = _Emission + _Light * phase * (1.0-_Absorption) * _Mie * p * atten;
            vec3 integral = (lum - (lum*trans))/ext;
            
            result.rgb += integral * result.a;
            vec3 div = vec3(0.3333333);
            result.a *= dot(trans, div);
            
            if (result.a <= 0.1)
                return result;
        }
            
        pos += dir * ds;
    }
        
    return result;
}



//4x4 Bayer matrix for ordered dithering
const mat4 _Bayer4x4 = mat4(vec4(0,0.5,0.125,0.625),
                        vec4(0.75,0.25,0.875,.375), 
                        vec4(0.1875,0.6875,0.0625,0.5625), 
                        vec4(0.9375,0.4375,0.8125,0.3125));


float dirtClouds(vec2 pos)
{
    float qwe = max(1./pow(abs(0.5 - max(pos.y, 1.0)), 1.5), 1.0); 
	float intensity = 1.0 - abs(0.5 - pos.y);
	return (fbm(pos * 0.1) * qwe + 1.0) / 2.0 * pow(intensity, 8.0);
}

void main()
{
    vec3 rayOrigin = vec3(0.0, 0.0, -5.0 + (5.0 * 3.2));
    vec2 uv = v_tex_coords;
    float ar = screen_width/screen_height;
    float d = ar/tan(radians(45.0/2.0));    
    vec3 rayDir = normalize(vec3((-1.0 + 2.0 * uv) * vec2(ar, 1.0), d));

    float t = 1.0 * 0.01;

    rayDir = rotateY(rayDir, t);
    rayOrigin = rotateY(rayOrigin, t);

    vec4 col = vec4(0.0,0.0,0.0,1.0);
    float star = smoothstep(0.45, 0.8, abs(noise(rayDir * 256.0)));
    col.rgb += star * mix(vec3(1.0,0.7,0.2),vec3(0.0,0.5,1.0), star);

    vec3 p0, p1;
    float radius = 1.0;
    if(draw_nebula == 1) {
    if (raycastSphere(rayOrigin, rayDir, p0, p1, vec3(0.0), radius))
    {
        p0 = (dot(rayOrigin, rayOrigin) < dot(p0, p0)) ? rayOrigin : p0;

        float width = (uv.x * screen_width);
        float height = (uv.y * screen_height);
        width = mod(width, 4.0);
        height = mod(height, 4.0);
        float offset = _Bayer4x4[int(width)][int(height)];
        p0 -= rayDir * offset * STEP_SIZE*2.0;

        float dist = length(p1 - p0);
        int s = int(dist/STEP_SIZE) + 1;

        vec4 integral = raymarch(p0, rayDir, STEP_SIZE, s);

        col.rgb = mix(integral.rgb, col.rgb, integral.a);
    }
    }

    frag_color = vec4(col.rgb, 1.0);
    
	//frag_color.rgb = bgGlow(uv) + makeStars(uv) +  (brightClouds(uv) + darkClouds(uv)) / 0.5 + dirt(uv);
    //frag_color.rgb = mix(frag_color.rgb, vec3(0.125, 0.1, 0.1), dirtClouds(uv) * 0.75 * pow(length(uv * 2.0 - 1.0), 0.25));

    //float f = dirtClouds(uv);
    //frag_color = vec4(f, f, f, 1.0);
    
    frag_color.a = 1.0;
}


//#version 450
//
//out vec4 frag_color;
//
//in vec2 tex_coords;
//
////uniform sampler2D screenTexture;
//
//void main()
//{
//    vec3 color = texture(screenTexture, tex_coords).rgb;
//    FragColor = vec4(color, 1.0);
//}