#ifndef PLANET_H
#define PLANET_H

#include <windows.h>
#include <stdio.h>
#include "waves/waves.h"
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include "../geometry/geometry.h"
#include "../graphics/shader.h"
#include "../graphics/buffer.h"
#include "../camera/camera.h"
#include "../matrixmath/matrixMath.h"
#include "../simplexnoise/simplexnoise.h"

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

planet_t *planet_init();
void planet_draw(planet_t planet, unsigned int shader, arcball_camera camera, float time);

#endif