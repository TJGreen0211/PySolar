#ifndef STARSYSTEM_H
#define STARSYSTEM_H

#include <windows.h>
#include <stdio.h>
#include "planet.h"
#include "waves/waves.h"
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include "../geometry/geometry.h"
#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../camera/camera.h"
#include "../matrixmath/matrixMath.h"
#include "../simplexnoise/simplexnoise.h"

typedef struct starsystem {
    unsigned int sun_shader;
    unsigned int planet_shader;
    unsigned int atmosphere_shader;
    unsigned int sky_shader;
    unsigned int skybox_shader;
    unsigned int wave_shader;
    planet_t *planets;
    buffer sphere_buffer;
    simplexnoise snoise;
} starsystem;

starsystem *starsystem_init();
void starsystem_draw(starsystem *s, arcball_camera camera, float time, int width, int height, unsigned int framebuffer);

#endif