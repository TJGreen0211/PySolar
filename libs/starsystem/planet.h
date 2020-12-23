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

typedef struct planet_draw_params
{
    int lod_count;
    unsigned int model_translations_buffer;
    unsigned int model_transformation_buffer;
} planet_draw_params;


typedef struct planet_t planet_t;

typedef struct planet_t {
    float radius;
    float atmosphere_radius;
    float semimajor_axis;
    float orbital_period;
    float orbital_speed;
    float axial_tilt;
    int has_atmosphere;
    int lod_count;
    unsigned int position_buffer;
    planet_draw_params draw_params; 
    atmosphere_parameters atmosphere;
    buffer sphere_faces[6];
    simplexnoise snoise_face[6];
    simplexnoise snoise_biomes[6];
    buffer planet_ring;
    buffer ring_buffer;
    planet_t *moons;
    waves_t waves;
} planet_t;

planet_t *planet_init();
void planet_draw(planet_t planet, unsigned int shader, arcball_camera camera, float time, unsigned int framebuffer);

#endif