#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coords;
layout(location = 3) in vec3 v_camera_position;
layout(location = 4) in vec3 v_light_position;

uniform sampler2D texture1;
//uniform sampler2D normal_map;
//uniform sampler2D specular_map;
//uniform sampler2D depthmap;
uniform float time;

const vec3 diffuseColor = vec3(0.8, 0.8, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float heightScale = 0.01;


vec2 hash2( vec2 p )
{
	// texture based white noise
//	return texture( iChannel0, (p+0.5)/256.0, -100.0 ).xy;
	
    // procedural white noise	
	return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}


vec3 voronoi( in vec2 x )
{
    vec2 n = floor(x);
    vec2 f = fract(x);

    //----------------------------------
    // first pass: regular voronoi
    //----------------------------------
	vec2 mg, mr;

    float md = 8.0;
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2 g = vec2(float(i),float(j));
		vec2 o = hash2( n + g );
        o = 0.5 + 0.5*sin( time + 6.2831*o );
        vec2 r = g + o - f;
        float d = dot(r,r);

        if( d<md )
        {
            md = d;
            mr = r;
            mg = g;
        }
    }

    //----------------------------------
    // second pass: distance to borders
    //----------------------------------

    md = 8.0;
    for( int j=-2; j<=2; j++ )
    for( int i=-2; i<=2; i++ )
    {
        vec2 g = mg + vec2(float(i),float(j));
		vec2 o = hash2( n + g );
        o = 0.5 + 0.5*sin( time + 6.2831*o );
        vec2 r = g + o - f;

        if( dot(mr-r,mr-r)>0.00001 )
        md = min( md, dot( 0.5*(mr+r), normalize(r-mr) ) );
    }


    return vec3( md, mr );
}


void main()
{
	vec3 view_dir = normalize(v_camera_position - v_position);

	vec3 normal = normalize(v_normal);

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

    //FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
	vec3 frag_color = vec3(lambertian*(diffuseColor) + specular*(specColor));

	float t = time/0.1;
	float scale1 = 40.0;
    float scale2 = 20.0;
    float val = 0.0;

	val += sin((v_position.x*scale1 + t));
    val += sin((v_position.y*scale1 + t)/2.0);
    val += sin((v_position.x*scale2 + v_position.z*scale2 + sin(t))/2.0);
    val += sin((v_position.x*scale2 - v_position.z*scale2 + t)/2.0);
    val /= 2.0;

	vec3 col2 = vec3(0.3, 0.7, 1.0);
	gl_FragColor = vec4(col2*val, 1.0);
	//gl_FragColor = vec4(pow(frag_color,vec3(1.0/gamma)), 1.0);
	//gl_FragColor = vec4(color, 1.0);
}