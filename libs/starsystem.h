#ifndef STARSYSTEM_H
#define STARSYSTEM_H

#include <windows.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include "geometry/geometry.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "camera/camera.h"
#include "waves/waves.h"
#include "matrixmath/matrixMath.h"
#include "simplexnoise/simplexnoise.h"

typedef struct planet_t {
    float radius;
    float atmosphere_radius;
    buffer sphere_faces[6];
    simplexnoise snoise_face[6];
    simplexnoise snoise_biomes[6];
    unsigned int snoise_textures[0];
    buffer planet_ring;
    waves_t waves;
} planet_t;

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