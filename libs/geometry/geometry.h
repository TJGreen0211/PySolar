#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../matrixmath/matrixMath.h"

#ifdef _WIN32
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT
#endif


typedef struct geometry {
	int vertex_number;
	int point_size;
	int normal_size;
	int tangent_size;
    int tex_coord_size;
    vec3 *points;
	vec3 *normals;
	vec3 *tangents;
	vec2 *tex_coords;
} geometry;

void geometry_quadcube_create(int divisions, geometry *quadcube);
void geometry_quadcube_create_face(geometry *quadcube_face, int divisions, int order[3], int reverse);
void geometry_quadcube_dealloc(geometry *quadcube);

void geometry_ring_create(int divisions, geometry *ring);
void geometry_load_object_file(const char *fname, geometry *mesh);
void geometry_load_object_file_depricated(const char *fname, geometry *mesh);
void geometry_mesh_dealloc(geometry *mesh);

#endif