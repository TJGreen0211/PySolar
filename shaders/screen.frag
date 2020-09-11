#version 450

layout(location = 0) in vec2 f_tex_coords;

uniform sampler2D screenTexture;

void main()
{
    vec3 color = texture(screenTexture, f_tex_coords).rgb;
    gl_FragColor = vec4(color, 1.0);
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