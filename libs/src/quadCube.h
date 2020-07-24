#ifndef QUADCUBE_H
#define QUADCUBE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "matrixMath.h"

typedef struct quadCube {
	int size;
	int nsize;
	int vertexNumber;
    vec3 *points;
	vec3 *normals;
} quadCube;

void createCube(int divisions, quadCube *newQuadCube);
void deallocCube(quadCube *newQuadCube);

#endif