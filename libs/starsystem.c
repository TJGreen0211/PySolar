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


static void draw_moon(starsystem *s, arcball_camera camera, float time) {
    glUseProgram(s->planet_shader);

	float trx = (500.0) * cos(time);
	float try = 0.0;
	float trz = (500.0) * sin(time);

	//mat4 model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(translation)), scale(10.0)),rotateX(90.0));

    mat4 model = mat4Multiply(mat4Multiply(mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4Translate(trx, try, trz, 1.0)), mat4ScaleScalar(50.0)), mat4RotateY(time*5.0));
    glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    vec4 camera_position = getCameraPosition(camera, model);
    glUniform3f(glGetUniformLocation(s->planet_shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(s->planet_shader, "lightPosition"), 10.0, 5.0, -4.0);
    glUniform1f(glGetUniformLocation(s->planet_shader, "time"), time);

    // Noise Texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
    glUniform1i(glGetUniformLocation(s->planet_shader, "texture1"), 1);

    // Update wave textures from the genereated wave patch
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dx_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdx);
    glUniform1i(glGetUniformLocation(s->planet_shader, "wave_dx"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dy_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdy);
    glUniform1i(glGetUniformLocation(s->planet_shader, "wave_dy"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dz_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdz);
    glUniform1i(glGetUniformLocation(s->planet_shader, "wave_dz"), 4);

    glActiveTexture(GL_TEXTURE0);



    for(int i = 0; i < 6; i++) {
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(s->planets[0].sphere_faces[i].vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
	    glDrawArrays(GL_PATCHES, 0, s->planets[0].sphere_faces[i].vertex_number);
    }

	//glDrawArrays(GL_TRIANGLES, 0, s->sphere_buffer.vertex_number);
    glBindVertexArray(0);
}

//static float point_distance(float u[3], float v[3]) {
//	return sqrt((v[0] - u[0])*(v[0] - u[0]) + (v[1] - u[1])*(v[1] - u[1]) + (v[2] - u[2])*(v[2] - u[2]));
//}

static void draw_planet(starsystem *s, arcball_camera camera, float time) {
    glUseProgram(s->planet_shader);

    mat4 model = mat4Multiply(mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4ScaleScalar(s->planets[0].radius)), mat4RotateY(1.0));
    glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    vec4 camera_position = getCameraPosition(camera, model);
    glUniform3f(glGetUniformLocation(s->planet_shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(s->planet_shader, "lightPosition"), 10.0, 5.0, -4.0);
    glUniform1f(glGetUniformLocation(s->planet_shader, "time"), time);

    // Noise Texture
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
    //glBindTexture(GL_TEXTURE_2D, s->planets[0].snoise_textures[i]);
    glUniform1i(glGetUniformLocation(s->planet_shader, "texture1"), 1);

    // Update wave textures from the genereated wave patch
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dx_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdx);
    glUniform1i(glGetUniformLocation(s->planet_shader, "wave_dx"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dy_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdy);
    glUniform1i(glGetUniformLocation(s->planet_shader, "wave_dy"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dz_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdz);
    glUniform1i(glGetUniformLocation(s->planet_shader, "wave_dz"), 4);

    glActiveTexture(GL_TEXTURE0);


    for(int i = 0; i < 6; i++) {
        glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
        glBindTexture(GL_TEXTURE_2D, s->planets[0].snoise_face[i].render_texture);
        glActiveTexture(GL_TEXTURE0);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(s->planets[0].sphere_faces[i].vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
	    glDrawArrays(GL_PATCHES, 0, s->planets[0].sphere_faces[i].vertex_number);
    }
    
/*
    //int order_array[6][3] = {
    //    {0, 1, 2},
    //    {0, 1, 2},
    //    {2, 0, 1},
    //    {2, 0, 1},
    //    {1, 2, 0},
    //    {1, 2, 0}
    //};
    
    float quad_scale = 0.5;
    float model_scale = 350.0;
    float center[3] = {0.0, 0.0, 0.0};
    camera_position = getCameraPosition(camera, model);

    vec2 asdf = {{camera_position.v[1], camera_position.v[2]}};
    asdf = vec2Normalize(asdf);

    //printf("Dist: %f, cam: %f, %f, %f normalized: {%f, %f}\n",  camera_position.v[0], camera_position.v[1], camera_position.v[2], asdf.v[0], asdf.v[1]);
    float cam[3] = {camera_position.v[0], camera_position.v[1], camera_position.v[2]};
    //for(int i = 0; i < 2; i++) {
        //float translate_array[4][3] = {
        //    {(center[0] + quad_scale)*model_scale, (center[1] - quad_scale)*model_scale, 0.0},
        //    {(center[0] + quad_scale)*model_scale, (center[1] + quad_scale)*model_scale, 0.0},
        //    {(center[0] - quad_scale)*model_scale, (center[1] - quad_scale)*model_scale, 0.0},
        //    {(center[0] - quad_scale)*model_scale, (center[1] + quad_scale)*model_scale, 0.0},
        //};
        float translate_array[4][3] = {
            {(center[0] + quad_scale)*s->planets[0].radius, (center[1] - quad_scale)*s->planets[0].radius, 0.0},
            {(center[0] + quad_scale)*s->planets[0].radius, (center[1] + quad_scale)*s->planets[0].radius, 0.0},
            {(center[0] - quad_scale)*s->planets[0].radius, (center[1] - quad_scale)*s->planets[0].radius, 0.0},
            {(center[0] - quad_scale)*s->planets[0].radius, (center[1] + quad_scale)*s->planets[0].radius, 0.0},
        };


        float center_model_translation[3] = {center[0]-model_scale, center[1], center[2]};
        //printf("Dist: %f, cam: %f, %f, %f normalized: {%f, %f}\n", point_distance(cam, center_model_translation), camera_position.v[0], camera_position.v[1], camera_position.v[2], asdf.v[0], asdf.v[1]);
        //int detail_level = (int)(log(512)/log(point_distance(cam, center)));
        // Exponential Decay
        // A = A0*e^(kt)
        int detail_level = 0;
        //printf("LOD: %d\n", detail_level);
        
        if(point_distance(cam, center_model_translation) != 0.0) {
            //printf("Dist: %f\n", point_distance(cam, center));
            for(int j = 0; j < 4; j++) {
                //printf("%f, %f\n",translate_array[j][0], translate_array[j][1]);
                //mat4 model = mat4Multiply(mat4Multiply(mat4Translate(model_scale, 0.0, 0.0, 1.0), mat4ScaleScalar(s->planets[0].radius)), mat4RotateY(1.0));
                mat4 model = mat4Multiply(
                    mat4Multiply(
                        mat4Translate(model_scale, translate_array[j][0], translate_array[j][1], 1.0), 
                        mat4Scale(s->planets[0].radius, quad_scale*s->planets[0].radius, quad_scale*s->planets[0].radius, 1.0)
                    ), mat4RotateY(1.0)
                );
                //mat4 model = mat4Translate(model_scale, translate_array[j][0], translate_array[j][1], 1.0);
                //mat4 model = mat4Scale(s->planets[0].radius, quad_scale*s->planets[0].radius, quad_scale*s->planets[0].radius, 1.0);
                //mat4 position = mat4Multiply(
                //    //mat4Translate(values[order_array[5][0]], values[order_array[5][1]], values[order_array[5][2]], 1.0), 
                //    //mat4Scale(scale_values[order_array[5][0]], scale_values[order_array[5][1]], scale_values[order_array[5][2]], 1.0)
                //    mat4Translate(0.0, translate_array[j][0], translate_array[j][1], 1.0),
                //    mat4Scale(1.0, quad_scale, quad_scale, 1.0)
                //);
//
                //model = mat4Multiply(model, position);
                glUniformMatrix4fv(glGetUniformLocation(s->planet_shader, "model"), 1, GL_FALSE, &model.m[0][0]);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, s->planets[0].snoise_face[5].render_texture);
                glActiveTexture(GL_TEXTURE0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glBindVertexArray(s->planets[0].sphere_faces[5].vao);
                glPatchParameteri(GL_PATCH_VERTICES, 3);
	            glDrawArrays(GL_PATCHES, 0, s->planets[0].sphere_faces[5].vertex_number);
            }
        //}
        center[0] = center[0] - quad_scale;
        center[1] = center[1] - quad_scale;
        quad_scale /= 2.0;

        //printf("i: %d, center: {%f, %f}, scale: %f\n", i, center[0], center[1], quad_scale);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
*/
	//glDrawArrays(GL_TRIANGLES, 0, s->sphere_buffer.vertex_number);
    glBindVertexArray(0);
}
/*
static void draw_waves(starsystem *s, arcball_camera camera, float time) {
    glUseProgram(s->wave_shader);

    mat4 model = mat4Multiply(mat4ScaleScalar(s->planets[0].radius*1.01), mat4RotateY(time*5.0));
    glUniformMatrix4fv(glGetUniformLocation(s->wave_shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s->wave_shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s->wave_shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    vec4 camera_position = getCameraPosition(camera, model);
    glUniform3f(glGetUniformLocation(s->wave_shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(s->wave_shader, "lightPosition"), 10.0, 5.0, -4.0);
    glUniform1f(glGetUniformLocation(s->wave_shader, "time"), time);

    // Noise Texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
    glUniform1i(glGetUniformLocation(s->wave_shader, "texture1"), 1);

    // Update wave textures from the genereated wave patch
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dx_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdx);
    glUniform1i(glGetUniformLocation(s->wave_shader, "wave_dx"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dy_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdy);
    glUniform1i(glGetUniformLocation(s->wave_shader, "wave_dy"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, s->planets[0].waves.dz_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->planets[0].waves.dimension, s->planets[0].waves.dimension, GL_RGBA, GL_FLOAT, s->planets[0].waves.displacementdz);
    glUniform1i(glGetUniformLocation(s->wave_shader, "wave_dz"), 4);

    glActiveTexture(GL_TEXTURE0);

    for(int i = 0; i < 6; i++) {
        glBindVertexArray(s->planets[0].sphere_faces[i].vao);
	    glDrawArrays(GL_TRIANGLES, 0,  s->planets[0].sphere_faces[i].vertex_number);
        glBindVertexArray(0);
    }
}*/


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
    
    //
    waves_generate(&s->planets[0].waves, time);
    draw_planet(s, camera, time);
    draw_moon(s, camera, time);
    //draw_waves(s, camera, time);
    draw_atmosphere(s, camera, time);
}

void starsystem_dealloc(starsystem *s) {
    //free()
}