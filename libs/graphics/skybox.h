#ifndef SKYBOX_H
#define SKYBOX_H

#include <stdlib.h>
#include <stdio.h>
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include "buffer.h"
#include "shader.h"
#include "framebuffer.h"
#include "../camera/camera.h"

typedef struct skybox {
    int texture_size;
    unsigned int skybox_texture;
    unsigned int render_shader;
    unsigned int skybox_shader;
    unsigned int skybox_face_textures[6];
    buffer cube_buffer;
} skybox;

skybox skybox_init(int skybox_size);
void skybox_draw(skybox s, arcball_camera camera);

#endif