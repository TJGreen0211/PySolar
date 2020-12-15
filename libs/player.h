#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include "graphics/buffer.h"
#include "camera/camera.h"
#include "geometry/geometry.h"
#include "graphics/shader.h"

typedef struct player {
    unsigned int mesh_shader;
    buffer mesh_buffer;
    vec3 current_position;
    vec3 delta_position;
    float current_rotation;
    float delta_rotation;
} player;

player *player_init();
void player_draw(player *p, arcball_camera camera, float time, int width, int height, unsigned int framebuffer);

#endif