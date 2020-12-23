#include "planet.h"

static int faces_drawn = 0;

static float point_distance(float u[3], float v[3]) {
	return sqrt((v[0] - u[0])*(v[0] - u[0]) + (v[1] - u[1])*(v[1] - u[1]) + (v[2] - u[2])*(v[2] - u[2]));
}

planet_t *planet_init() {
    planet_t *planet = (planet_t *)malloc(sizeof(planet_t));

    for(int i = 0; i < 6; i++) {
        simplexnoise_init(&planet[0].snoise_face[i], 4096, 4096);
        simplexnoise_init(&planet[0].snoise_biomes[i], 512, 512);
    }

    geometry quadcube_face;
    int order[3] = {0, 1, 2};
    geometry_quadcube_create_face(&quadcube_face, 20, order, 1);
    planet[0].sphere_faces[0] = buffer_init(quadcube_face);
    geometry_quadcube_create_face(&quadcube_face, 20, order, -1);
    planet[0].sphere_faces[1] = buffer_init(quadcube_face);
    
    order[0] =2; order[1] = 0; order[2] = 1;
    geometry_quadcube_create_face(&quadcube_face, 20, order, 1);
    planet[0].sphere_faces[2] = buffer_init(quadcube_face);
    geometry_quadcube_create_face(&quadcube_face, 20, order, -1);
    planet[0].sphere_faces[3] = buffer_init(quadcube_face);

    order[0] = 1; order[1] = 2; order[2] = 0;
    geometry_quadcube_create_face(&quadcube_face, 20, order, 1);
    planet[0].sphere_faces[4] = buffer_init(quadcube_face);
    geometry_quadcube_create_face(&quadcube_face, 20, order, -1);
    planet[0].sphere_faces[5] = buffer_init(quadcube_face);
    geometry_quadcube_dealloc(&quadcube_face);

    
    planet[0].radius = 500.0;
    planet[0].atmosphere_radius = 350.0;

    glGenBuffers(1, &planet[0].position_buffer);
    glGenBuffers(1, &planet[0].draw_params.model_transformation_buffer);

    return planet;
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

static void planet_traverse_quadtree(quadtree_node *node, planet_t *planet, int order[3], float flip, mat4 *translations, vec3 *transformations) {
    if(quadtree_node_is_leaf(node)) {
        faces_drawn++;
        
        //printf("Current Count: %d\n", planet->lod_count);
        //quadtree_print_node(node);

        float translate_order[3] = {node->center.x, node->center.y, ((node->width/2.0)*flip)-(1.0)*flip};
        mat4 translation = mat4Multiply(mat4Translate(translate_order[order[0]], translate_order[order[1]], translate_order[order[2]], 1.0), mat4ScaleScalar(node->width/2.0));
        translations[planet->lod_count] = translation;

        //printf("width: %f, center: (%f, %f)\n", node->width, node->center.x, node->center.y);
        //printf("Scale: %f, Translation: (%f, %f, %f)\n", node->width/2.0, translate_order[0], translate_order[1], translate_order[2]);
        //printf("Translation: (%f, %f)\n", ((translate_order[0]+1.0)/2.0)-(node->width/4.0), ((translate_order[1]+1.0)/2.0)-(node->width/4.0));

        vec3 transformation = {{((translate_order[0]+1.0)/2.0)-(node->width/4.0), ((translate_order[1]+1.0)/2.0)-(node->width/4.0), node->width/2.0}};

        transformations[planet->lod_count] = transformation;
        planet->lod_count++;

        

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        planet_traverse_quadtree(node->nw, planet, order, flip, translations, transformations);
        planet_traverse_quadtree(node->ne, planet, order, flip, translations, transformations);
        planet_traverse_quadtree(node->sw, planet, order, flip, translations, transformations);
        planet_traverse_quadtree(node->se, planet, order, flip, translations, transformations);
    }
}

static void planet_initialize_quadtree_draw(planet_t planet, arcball_camera camera, mat4 model, unsigned int shader, float time) {
    int order_array[6][3] = {
        {0, 1, 2},
        {0, 1, 2},
        {1, 2, 0},
        {1, 2, 0},
        {2, 0, 1},
        {2, 0, 1},
    };

    int order_array2[6][3] = {
        {0, 1, 2},
        {0, 1, 2},
        {2, 0, 1},
        {2, 0, 1},
        {1, 2, 0},
        {1, 2, 0}
    };

    vec4 camera_position = getCameraPosition(camera, mat4IdentityMatrix());
    float cam[3] = {camera_position.v[0], camera_position.v[1], camera_position.v[2]};
    float flip = -1.0;
    faces_drawn = 0;
    //printf("Planetradius: %f\n", planet.radius);

    mat4 *translations = (mat4 *)malloc(sizeof(mat4)*128);
    vec3 *transformations = (vec3 *)malloc(sizeof(vec3)*128);
    glUseProgram(shader);
    for(int i = 0; i < 6; i++) {
        planet.lod_count = 0;
        //render_simplexnoise_texture(&planet.snoise_face[i], 1.1, 0.0, i*512.0, i*512.0, order_array2[i], flip);
        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        float rad_to_cam[3] = {0.0, 0.0, planet.radius*(-flip)};
        float center_model_translation[3] = {rad_to_cam[order_array[i][0]], rad_to_cam[order_array[i][1]], rad_to_cam[order_array[i][2]]};
        float distance_to_sphere = point_distance(cam, center_model_translation);
        //printf("Distance: %f, camera: (%f, %f, %f)\n", distance_to_sphere, cam[0], cam[1], cam[2]);
        //printf("%f, %f, %f\n", rad_to_cam[order_array[i][0]], rad_to_cam[order_array[i][1]], rad_to_cam[order_array[i][2]]);
        // Exponential Decay
        // A = A0*e^(kt)
        // A = MAX_DETAIL*exp(log(DECAY_RATE/DIST_TO_DECAY)*CURRENT_DISTANCE)
        int detail_level = 8*exp((log(0.80)/planet.radius)*(int)distance_to_sphere);
        detail_level = detail_level > 0 ? detail_level : 1;

        quadtree_point p;
        p.x = camera_position.v[order_array2[i][0]];
        p.y = camera_position.v[order_array2[i][1]];
        quadtree_node *qt = quadtree_create(planet.radius, detail_level, p);
        
        planet_traverse_quadtree(qt, &planet, order_array[i], flip, translations, transformations);

        glBindBuffer(GL_ARRAY_BUFFER, planet.draw_params.model_transformation_buffer);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);
        glBufferData(GL_ARRAY_BUFFER, planet.lod_count * sizeof(vec3), &transformations[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, planet.position_buffer);
	    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(0));
	    glEnableVertexAttribArray(5);
	    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(sizeof(vec4)));
	    glEnableVertexAttribArray(6);
	    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(2*sizeof(vec4)));
	    glEnableVertexAttribArray(7);
	    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(3*sizeof(vec4)));
	    glEnableVertexAttribArray(8);

	    glVertexAttribDivisor(5, 1);
	    glVertexAttribDivisor(6, 1);
	    glVertexAttribDivisor(7, 1);
	    glVertexAttribDivisor(8, 1);

	    glBufferData(GL_ARRAY_BUFFER, planet.lod_count * sizeof(mat4), &translations[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Update wave textures from the generated wave patch
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, planet.waves.dx_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, planet.waves.dimension, planet.waves.dimension, GL_RGBA, GL_FLOAT, planet.waves.displacementdx);
        glUniform1i(glGetUniformLocation(shader, "wave_dx"), 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, planet.waves.dy_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, planet.waves.dimension, planet.waves.dimension, GL_RGBA, GL_FLOAT, planet.waves.displacementdy);
        glUniform1i(glGetUniformLocation(shader, "wave_dy"), 3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, planet.waves.dz_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, planet.waves.dimension, planet.waves.dimension, GL_RGBA, GL_FLOAT, planet.waves.displacementdz);
        glUniform1i(glGetUniformLocation(shader, "wave_dz"), 4);
        
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model.m[0][0]);
        //glUniformMatrix4fv(glGetUniformLocation(shader, "translation"), 1, GL_FALSE, &translation.m[0][0]);

        vec4 camera_position = getCameraPosition(camera, model);
        glUniform3f(glGetUniformLocation(shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
        glUniform3f(glGetUniformLocation(shader, "lightPosition"), -10.0, 5.0, -4000.0);
        glUniform1f(glGetUniformLocation(shader, "time"), 1.0);

        glUniform1i(glGetUniformLocation(shader, "texture1"), 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, planet.snoise_face[i].render_texture);
        glActiveTexture(GL_TEXTURE0);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(planet.sphere_faces[i].vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
	    //glDrawArrays(GL_PATCHES, 0, planet.sphere_faces[face_index].vertex_number);

        glDrawArraysInstanced(GL_PATCHES, 0, planet.sphere_faces[i].vertex_number, planet.lod_count);

        flip *= -1.0;

        faces_drawn = 0;
    }
    free(translations);
    free(transformations);

	//glDrawArrays(GL_TRIANGLES, 0, s->sphere_buffer.vertex_number);
    glBindVertexArray(0);
}

static void planet_draw_moon(planet_t planet, arcball_camera camera, unsigned int shader, float time) {
    glUseProgram(shader);

	float trx = (planet.radius*2.0) * cos(time/5.0);
	float try = 0.0;
	float trz = (planet.radius*2.0) * sin(time/5.0);

	//mat4 model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(translation)), scale(10.0)),rotateX(90.0));

    mat4 model = mat4Multiply(mat4Multiply(mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4Translate(trx, try, trz, 1.0)), mat4ScaleScalar(50.0)), mat4RotateY(time*5.0));

    planet_initialize_quadtree_draw(planet, camera, model, shader, time);
}


void planet_draw(planet_t planet, unsigned int shader, arcball_camera camera, float time, unsigned int framebuffer) {
    /*mat4 model = mat4Multiply(mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4ScaleScalar(planet.radius)), mat4RotateY(1.0));*/
    mat4 model = mat4Multiply(mat4Translate(0.0, 0.0, 0.0, 1.0), mat4ScaleScalar(planet.radius));
    planet_initialize_quadtree_draw(planet, camera, model, shader, time);

    planet_draw_moon(planet, camera, shader, time);
}