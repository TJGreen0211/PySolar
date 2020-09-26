#ifndef WAVES_H
#define WAVES_H
#ifdef __APPLE__
#endif

#include <windows.h>
#include "../src/matrixMath.h"

typedef struct complexType {
	double real;
	double im;
} complexType;

typedef struct waves {
	int dimension;
	unsigned int dxTexture;
	unsigned int dyTexture;
	unsigned int dzTexture;
	double *displacementdx;
	double *displacementdy;
	double *displacementdz;
	complexType *tildeh0k;
	complexType *conjTildeh0MK;
} waves;

void initializeWaves(waves *water, int dim);
void generateWaves(waves *water, double currentTime);
void cleanupWaves(waves *water);

#endif