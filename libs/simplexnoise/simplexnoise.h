#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H

#include <windows.h>
#include <stdio.h>
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include "../graphics/shader.h"

typedef struct simplexnoise {
    unsigned int shader;
    unsigned int vao;
    unsigned int vbo;
    unsigned int perm_texture;
    unsigned int simplex_texture;
    unsigned int grad_texture;
    unsigned int noise_fboid;
    unsigned int noise_texid;
    unsigned int render_texture;
    float e[6];
    int vertex_number;
    int width;
    int height;
} simplexnoise;

void simplexnoise_init(simplexnoise *snoise, int width, int height);
void render_simplexnoise_texture(simplexnoise *snoise, float time, float seed_x, float seed_y, int order[3], int reverse);

#define BUFFER_OFFSET(offset) ((char*)NULL+(offset))

#endif