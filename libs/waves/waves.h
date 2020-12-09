#ifndef WAVES_H
#define WAVES_H
#ifdef _WIN32
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT
#endif


#include <windows.h>
#include "../matrixmath/matrixMath.h"

typedef struct complexType {
	float real;
	float im;
} complexType;

typedef struct waves_t {
	int dimension;
	unsigned int dx_texture;
	unsigned int dy_texture;
	unsigned int dz_texture;
	float *displacementdx;
	float *displacementdy;
	float *displacementdz;
	complexType *tildeh0k;
	complexType *conjTildeh0MK;
} waves_t;

void waves_init(waves_t *water, int dim);
void waves_generate(waves_t *water, float currentTime);
void waves_dealloc(waves_t *water);

#endif