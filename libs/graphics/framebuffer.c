#include "framebuffer.h"

unsigned int framebuffer_init_texture_attachment(int size_x, int size_y) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_id;
}

void framebuffer_init_screen(fbo *framebuffer) {
    framebuffer->framebuffer_shader = shader_create_program("../shaders/screen.vert",
        "../shaders/screen.frag",NULL,NULL,NULL);
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

    glGenVertexArrays(1, &framebuffer->quad_buffer.vao);
	glBindVertexArray(framebuffer->quad_buffer.vao);
    glGenBuffers(1, &framebuffer->quad_buffer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, framebuffer->quad_buffer.vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices)+sizeof(quad_tex_coords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_vertices), quad_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad_vertices), sizeof(quad_tex_coords), quad_tex_coords);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), BUFFER_OFFSET(sizeof(quad_vertices)));
	glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void framebuffer_init(float size_x, float size_y, fbo *framebuffer) {
    framebuffer->size_x = size_x;
    framebuffer->size_y = size_y;

    framebuffer_init_screen(framebuffer);

    glGenFramebuffers(1, &framebuffer->fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo_id);
    framebuffer->fbo_texture_id = framebuffer_init_texture_attachment(framebuffer->size_x, framebuffer->size_y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->fbo_texture_id, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebuffer->size_x, framebuffer->size_y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_draw(fbo *framebuffer) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(framebuffer->framebuffer_shader);
    glBindVertexArray(framebuffer->quad_buffer.vao);
    glBindTexture(GL_TEXTURE_2D, framebuffer->fbo_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void framebuffer_destroy(fbo *framebuffer) {
    glDeleteProgram(framebuffer->framebuffer_shader);
    glDeleteBuffers(1, framebuffer->quad_buffer.vbo);
}