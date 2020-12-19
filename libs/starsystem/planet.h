#ifndef PLANET_H
#define PLANET_H

#include <stdlib.h>
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
#include "../utility/quadtree/quadtree.h"

typedef struct atmosphere_parameters {
    float radius;
    float gravity;
    float c_r[3];
    float k_r;
    float k_m;
    float g_m;
    float e;
} atmosphere_parameters;

typedef struct planet_t {
    float radius;
    float atmosphere_radius;
    float semimajor_axis;
    float orbital_period;
    float orbital_speed;
    float axial_tilt;
    int has_atmosphere;
    atmosphere_parameters atmosphere;
    buffer sphere_faces[6];
    simplexnoise snoise_face[6];
    simplexnoise snoise_biomes[6];
    buffer planet_ring;
    waves_t waves;
} planet_t;

planet_t *planet_init();
void planet_draw(planet_t planet, unsigned int shader, arcball_camera camera, float time, unsigned int framebuffer);

#endif