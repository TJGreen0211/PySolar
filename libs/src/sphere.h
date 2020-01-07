#ifndef SPHERE_H
#define SPHERE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "matrixMath.h"

typedef struct sphere {
	int size;
	int nsize;
	int divisions;
	vec3 *points;
	vec3 *normals;
	int vertexNumber;
} sphere;

void tetrahedron(sphere *s, int num_divisions);
void deallocSphere(sphere *s);

#endif