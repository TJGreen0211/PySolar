#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdlib.h>
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include <stdio.h>
#include "buffer.h"
#include "shader.h"

typedef struct fbo {
    int size_x;
    int size_y;
    unsigned int fbo_id;
    unsigned int fbo_texture_id;
    unsigned int framebuffer_shader;
    buffer quad_buffer;
} fbo;

void framebuffer_init(float size_x, float size_y, fbo *framebuffer);
void framebuffer_draw(fbo *framebuffer);

#endif