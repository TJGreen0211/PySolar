#version 450

//uniform sampler2D texture1;
//uniform sampler2D texture2;
//uniform sampler2D texture3;

in vec3 v_color;
in vec2 v_tex_coords;

uniform sampler2D screen_texture;

void main()
{
    vec3 color = texture(screen_texture, v_tex_coords).rgb;
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