#include "starsystem.h"

static void load_shaders(starsystem *s);
static unsigned int create_wave_texture(int wave_patch_dimension);

starsystem *starsystem_init() {
    starsystem *s = (starsystem *)malloc(sizeof(starsystem));
    s->planets = malloc(sizeof(planet_t));
    if (s == NULL) {
        free(s);
        s = NULL;
    }
    load_shaders(s);

    geometry quad_sphere;
    geometry_quadcube_create(20, &quad_sphere);
    s->sphere_buffer = buffer_init(quad_sphere);
    geometry_quadcube_dealloc(&quad_sphere);

    int order[3] = {0, 1, 2};
    geometry_quadcube_create_face(&quad_sphere, 20, order, 1);
    s->planets[0].sphere_faces[0] = buffer_init(quad_sphere);
    geometry_quadcube_create_face(&quad_sphere, 20, order, -1);
    s->planets[0].sphere_faces[1] = buffer_init(quad_sphere);
    
    order[0] =2; order[1] = 0; order[2] = 1;
    geometry_quadcube_create_face(&quad_sphere, 20, order, 1);
    s->planets[0].sphere_faces[2] = buffer_init(quad_sphere);
    geometry_quadcube_create_face(&quad_sphere, 20, order, -1);
    s->planets[0].sphere_faces[3] = buffer_init(quad_sphere);

    order[0] = 1; order[1] = 2; order[2] = 0;
    geometry_quadcube_create_face(&quad_sphere, 20, order, 1);
    s->planets[0].sphere_faces[4] = buffer_init(quad_sphere);
    geometry_quadcube_create_face(&quad_sphere, 20, order, -1);
    s->planets[0].sphere_faces[5] = buffer_init(quad_sphere);
    geometry_quadcube_dealloc(&quad_sphere);

    s->planets[0].radius = 300.0;
    s->planets[0].atmosphere_radius = 350.0;

    float e[6] = {1.00, 0.50, 0.25, 0.13, 0.06, 0.03};
    for(int j = 0; j<6; j++) {
    for(int i = 0; i < 6; i ++) {
        s->planets[0].snoise_face[j].e[i] = e[i];
        s->planets[0].snoise_biomes[j].e[i] = e[i];
    }
    }

    for(int i = 0; i < 6; i++) {
        simplexnoise_init(&s->planets[0].snoise_face[i], 1024, 1024);
        simplexnoise_init(&s->planets[0].snoise_biomes[i], 512, 512);
    }
    int order_array[6][3] = {
        {0, 1, 2},
        {0, 1, 2},
        {2, 0, 1},
        {2, 0, 1},
        {1, 2, 0},
        {1, 2, 0}
    };
    int flip = 1;
    for(int i = 0; i < 6; i++) {
        render_simplexnoise_texture(&s->planets[0].snoise_face[i], 0.0, i*s->snoise.width, i*s->snoise.height, order_array[i], flip);
        render_simplexnoise_texture(&s->planets[0].snoise_biomes[i], 0.0, i*s->snoise.width, i*s->snoise.height, order_array[i], flip);
        //s->planets[0].snoise_textures[i] = s->snoise.render_texture;
        flip *= -1;
    }

    simplexnoise_init(&s->snoise, 1024, 1024);
    waves_init(&s->planets[0].waves, 128);
    s->planets[0].waves.dx_texture = create_wave_texture(s->planets[0].waves.dimension);
    s->planets[0].waves.dy_texture = create_wave_texture(s->planets[0].waves.dimension);
    s->planets[0].waves.dz_texture = create_wave_texture(s->planets[0].waves.dimension);

    return s;
}


static void load_shaders(starsystem *s) {
    s->sun_shader = shader_create_program("../shaders/star.vert",
        "../shaders/star.frag",NULL,NULL,NULL);
    s->wave_shader = shader_create_program("../shaders/planet2.vert",
        "../shaders/planet2.frag",NULL,NULL,NULL);
    s->atmosphere_shader = shader_create_program("../shaders/atmosphere.vert",
        "../shaders/atmosphere.frag",NULL,NULL,NULL);
    s->sky_shader = shader_create_program("../shaders/sky.vert",
        "../shaders/sky.frag",NULL,NULL,NULL);

    s->planet_shader = shader_create_program(
        "../shaders/planet.vert",
        "../shaders/planet.frag",
        "../shaders/planet.tcsh",
        "../shaders/planet.tesh",
        NULL);
}


static unsigned int create_wave_texture(int wave_patch_dimension) {
    unsigned int texid;
    glGenTextures(1, &texid);

    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wave_patch_dimension, wave_patch_dimension, 0, GL_RGBA, GL_FLOAT, NULL);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texid;
}


static void draw_atmosphere(starsystem *s, arcball_camera camera, float time) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    float atmosphere_scale_value = s->planets[0].atmosphere_radius;

    mat4 model = mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4ScaleScalar(atmosphere_scale_value));
    vec4 camera_position = getCameraPosition(camera, model);

    unsigned int shader_program;
    if(camera_position.v[0]*camera_position.v[0] + camera_position.v[1]*camera_position.v[1] + camera_position.v[2]*camera_position.v[2] < model.m[0][0]*model.m[0][0]) {
        shader_program = s->sky_shader;
		glCullFace(GL_FRONT);
	}
	else {
        shader_program = s->atmosphere_shader;
    }
    glUseProgram(shader_program);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, &model.m[0][0]);

    glUniform3f(glGetUniformLocation(shader_program, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(shader_program, "lightPosition"), 10.0, 5.0, -4.0);

    // Atmosphere constants
    glUniform1f(glGetUniformLocation(shader_program, "fInnerRadius"), s->planets[0].radius);
    glUniform1f(glGetUniformLocation(shader_program, "fOuterRadius"), atmosphere_scale_value);
    glUniform3f(glGetUniformLocation(shader_program, "C_R"), 0.3, 0.1, 1.0);
    glUniform1f(glGetUniformLocation(shader_program, "K_R"), 0.166);
    glUniform1f(glGetUniformLocation(shader_program, "K_M"), 0.0025);
    glUniform1f(glGetUniformLocation(shader_program, "G_M"), -0.85);
    glUniform1f(glGetUniformLocation(shader_program, "E"), 14.3);

    glBindVertexArray(s->sphere_buffer.vao);
	glDrawArrays(GL_TRIANGLES, 0, s->sphere_buffer.vertex_number);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glCullFace(GL_BACK);
}


void starsystem_draw(starsystem *s, arcball_camera camera, float time, int width, int height, unsigned int framebuffer) {
    glViewport(0,0, width, height); 

    /*int order[3] = {0, 1, 2};
    render_simplexnoise_texture(&s->snoise, time, s->snoise.width, s->snoise.height, order, 1);
    s->planets[0].snoise_textures[0] = s->snoise.render_texture;
    s->planets[0].snoise_textures[1] = s->snoise.render_texture;
    order[0] =2; order[1] = 0; order[2] = 1;
    render_simplexnoise_texture(&s->snoise, time, s->snoise.width, s->snoise.height, order, 1);
    s->planets[0].snoise_textures[2] = s->snoise.render_texture;
    s->planets[0].snoise_textures[3] = s->snoise.render_texture;
    order[0] = 1; order[1] = 2; order[2] = 0;
    render_simplexnoise_texture(&s->snoise, time, s->snoise.width, s->snoise.height, order, 1);
    s->planets[0].snoise_textures[4] = s->snoise.render_texture;
    s->planets[0].snoise_textures[5] = s->snoise.render_texture;*/

    //glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    waves_generate(&s->planets[0].waves, time);
    planet_draw(s->planets[0], s->planet_shader, camera, time);
    //draw_waves(s, camera, time);
    draw_atmosphere(s, camera, time);
}

void starsystem_dealloc(starsystem *s) {
    //free()
}