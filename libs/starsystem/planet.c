#include "planet.h"


//static float point_distance(float u[3], float v[3]) {
//	return sqrt((v[0] - u[0])*(v[0] - u[0]) + (v[1] - u[1])*(v[1] - u[1]) + (v[2] - u[2])*(v[2] - u[2]));
//}

static void planet_draw_moon(planet_t p, arcball_camera camera, unsigned int shader, float time) {
    glUseProgram(shader);

	float trx = (500.0) * cos(time);
	float try = 0.0;
	float trz = (500.0) * sin(time);

	//mat4 model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(translation)), scale(10.0)),rotateX(90.0));

    mat4 model = mat4Multiply(mat4Multiply(mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4Translate(trx, try, trz, 1.0)), mat4ScaleScalar(50.0)), mat4RotateY(time*5.0));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    vec4 camera_position = getCameraPosition(camera, model);
    glUniform3f(glGetUniformLocation(shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(shader, "lightPosition"), 1000.0, 5.0, -4.0);
    glUniform1f(glGetUniformLocation(shader, "time"), time);

    // Noise Texture
    glUniform1i(glGetUniformLocation(shader, "texture1"), 1);

    // Update wave textures from the genereated wave patch
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, p.waves.dx_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, p.waves.dimension, p.waves.dimension, GL_RGBA, GL_FLOAT, p.waves.displacementdx);
    glUniform1i(glGetUniformLocation(shader, "wave_dx"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, p.waves.dy_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, p.waves.dimension, p.waves.dimension, GL_RGBA, GL_FLOAT, p.waves.displacementdy);
    glUniform1i(glGetUniformLocation(shader, "wave_dy"), 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, p.waves.dz_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, p.waves.dimension, p.waves.dimension, GL_RGBA, GL_FLOAT, p.waves.displacementdz);
    glUniform1i(glGetUniformLocation(shader, "wave_dz"), 4);

    glActiveTexture(GL_TEXTURE0);

    for(int i = 0; i < 6; i++) {
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
        glBindTexture(GL_TEXTURE_2D, p.snoise_face[i].render_texture);
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(p.sphere_faces[i].vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
	    glDrawArrays(GL_PATCHES, 0, p.sphere_faces[i].vertex_number);
    }

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


void planet_draw(planet_t planet, unsigned int shader, arcball_camera camera, float time) {
    glUseProgram(shader);

    mat4 model = mat4Multiply(mat4Multiply(mat4Translate(350.0, 0.0, 0.0, 1.0), mat4ScaleScalar(planet.radius)), mat4RotateY(1.0));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    vec4 camera_position = getCameraPosition(camera, model);
    glUniform3f(glGetUniformLocation(shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(shader, "lightPosition"), 1000.0, 5.0, -4.0);
    glUniform1f(glGetUniformLocation(shader, "time"), time);

    // Noise Texture
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
    //glBindTexture(GL_TEXTURE_2D, planet.snoise_textures[i]);
    glUniform1i(glGetUniformLocation(shader, "texture1"), 1);

    // Update wave textures from the genereated wave patch
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

    glActiveTexture(GL_TEXTURE0);


    for(int i = 0; i < 6; i++) {
        glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, s->snoise.render_texture);
        glBindTexture(GL_TEXTURE_2D, planet.snoise_face[i].render_texture);
        glActiveTexture(GL_TEXTURE0);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(planet.sphere_faces[i].vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
	    glDrawArrays(GL_PATCHES, 0, planet.sphere_faces[i].vertex_number);
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
            {(center[0] + quad_scale)*planet.radius, (center[1] - quad_scale)*planet.radius, 0.0},
            {(center[0] + quad_scale)*planet.radius, (center[1] + quad_scale)*planet.radius, 0.0},
            {(center[0] - quad_scale)*planet.radius, (center[1] - quad_scale)*planet.radius, 0.0},
            {(center[0] - quad_scale)*planet.radius, (center[1] + quad_scale)*planet.radius, 0.0},
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
                //mat4 model = mat4Multiply(mat4Multiply(mat4Translate(model_scale, 0.0, 0.0, 1.0), mat4ScaleScalar(planet.radius)), mat4RotateY(1.0));
                mat4 model = mat4Multiply(
                    mat4Multiply(
                        mat4Translate(model_scale, translate_array[j][0], translate_array[j][1], 1.0), 
                        mat4Scale(planet.radius, quad_scale*planet.radius, quad_scale*planet.radius, 1.0)
                    ), mat4RotateY(1.0)
                );
                //mat4 model = mat4Translate(model_scale, translate_array[j][0], translate_array[j][1], 1.0);
                //mat4 model = mat4Scale(planet.radius, quad_scale*planet.radius, quad_scale*planet.radius, 1.0);
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
                glBindTexture(GL_TEXTURE_2D, planet.snoise_face[5].render_texture);
                glActiveTexture(GL_TEXTURE0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glBindVertexArray(planet.sphere_faces[5].vao);
                glPatchParameteri(GL_PATCH_VERTICES, 3);
	            glDrawArrays(GL_PATCHES, 0, planet.sphere_faces[5].vertex_number);
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

    planet_draw_moon(planet, camera, shader, time);
}