#include "skybox.h"

static void geometry_quad_init(buffer *buf) {
    
    float quad_tex_coords[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    float quad_vertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    glGenVertexArrays(1, &buf->vao);
	glBindVertexArray(buf->vao);
    glGenBuffers(1, &buf->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buf->vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices)+sizeof(quad_tex_coords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_vertices), quad_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad_vertices), sizeof(quad_tex_coords), quad_tex_coords);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(sizeof(quad_vertices)));
	glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static unsigned int skybox_load_cubemap(skybox *s, unsigned int image_tex_id)
{
	unsigned int textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    unsigned char* pixels = (unsigned char *)malloc(sizeof(char)*s->texture_size*s->texture_size*4);

    glBindTexture(GL_TEXTURE_2D, image_tex_id);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, s->texture_size, s->texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    	
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(pixels);

    return textureID;
}



skybox skybox_init(int skybox_size) {
    skybox s;
    s.texture_size = skybox_size;
    s.skybox_shader = shader_create_program("../shaders/skybox.vert",
        "../shaders/skybox.frag",NULL,NULL,NULL);
    
    unsigned int skybox_image_shader = shader_create_program("../shaders/skybox_image.vert",
        "../shaders/skybox_image.frag",NULL,NULL,NULL);

    
    fbo skybox_image_framebuffer;
    framebuffer_init((float)s.texture_size, (float)s.texture_size, &skybox_image_framebuffer);
    buffer quad_buffer;
    geometry_quad_init(&quad_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, skybox_image_framebuffer.fbo_id);
        glViewport(0,0,s.texture_size,s.texture_size);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(skybox_image_shader);
        glUniform1f(glGetUniformLocation(skybox_image_shader, "time"), 1.0);
        glUniform1f(glGetUniformLocation(skybox_image_shader, "screen_width"), (float)s.texture_size);
        glUniform1f(glGetUniformLocation(skybox_image_shader, "screen_height"), (float)s.texture_size);
        glUniform1i(glGetUniformLocation(skybox_image_shader, "draw_nebula"), 1);
        glBindVertexArray(quad_buffer.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(quad_buffer.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffer_draw(&skybox_image_framebuffer);

    
    s.skybox_texture = skybox_load_cubemap(&s, skybox_image_framebuffer.fbo_texture_id);


	float skybox_vertices[] = {
        // Positions          
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    glGenVertexArrays(1, &s.cube_buffer.vao);
	glBindVertexArray(s.cube_buffer.vao);
    glGenBuffers(1, &s.cube_buffer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s.cube_buffer.vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(skybox_vertices), skybox_vertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return s;
    
}

void skybox_draw(skybox s, arcball_camera camera)
{
	glUseProgram(s.skybox_shader);
	
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
    
    mat4 model = mat4Translate(camera.position.v[0], camera.position.v[1], camera.position.v[2], 1.0);//mat4ScaleScalar(30.0);

    glUniformMatrix4fv(glGetUniformLocation(s.skybox_shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s.skybox_shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(s.skybox_shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    glBindVertexArray (s.cube_buffer.vao);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(s.skybox_shader, "skybox"), 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, s.skybox_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    
    glDepthMask(GL_TRUE);
}