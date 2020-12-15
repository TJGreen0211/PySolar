#include "geometry.h"

void geometry_quad_init(buffer *buf) {
    skybox_image_shader = shader_create_program("../shaders/skybox_image.vert",
        "../shaders/skybox_image.frag",NULL,NULL,NULL);
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