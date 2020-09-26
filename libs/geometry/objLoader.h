#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/matrixMath.h"

typedef struct obj{
	int size;
	int nsize;
	int vertexNumber;
	vec3 *points;
	vec3 *normals;
	vec3 *tangents;
} obj;

void load_object_file(char *fname, obj *mesh);

#endif