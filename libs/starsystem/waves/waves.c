/*
	* Author: Tyler Green
	This module uses 2d inverse fast Fourier transform to generate waves.

	Public functions:
	void initializeWaves(waves *water, int dim);
		Initializes the waves with the desired dimension
	void waves_generate(waves *water, currentTime);
		Called per frame to generate the x, y, and z textures
	void cleanupWaves(waves *water);
		Cleanup the waves

*/

//cl /W4 /LD / TC water.c

#include "waves.h"
#include <stdio.h>
#include <math.h>

complexType complexMult(complexType c0, complexType c1) {
	complexType c;
	c.real = c0.real * c1.real - c0.im * c1.im;
	c.im = c0.real * c1.im + c0.im * c1.real;
	return c;
}

complexType complexAdd(complexType c0, complexType c1) {
	complexType c;
	c.real = c0.real + c1.real;
	c.im = c0.im + c1.im;
	return c;
}

complexType complexConj(complexType c) {
	complexType cConj;
	cConj.real = c.real;
	cConj.im = -c.im;
	return cConj;
}

/*static int is_infinity_is_zero_is_denormal(float f) {
  if (f == 0.0f) return 0;
  if (isinf(f)) return 1;
  if (isnan(f)) return 1;
  if (isnormal(f)) return 0;  // neither zero, subnormal, infinite, nor NaN

  // All that is left is de-normal/sub-normal.
  return 1;
}*/

static vec4 gauss_rand() {
	float noise00 = (float)rand()/(RAND_MAX);
	float noise01 = (float)rand()/(RAND_MAX);
	float noise02 = (float)rand()/(RAND_MAX);
	float noise03 = (float)rand()/(RAND_MAX);

	float u0 = 2.0*M_PI*noise00;
	float v0 = (noise01 == 0.0) ? 0.0 : sqrt(-2.0 * log(noise01));
	float u1 = 2.0*M_PI*noise02;
	float v1 = (noise03 == 0.0) ? 0.0 : sqrt(-2.0 * log(noise03));
	vec4 grnd = {{v0*cos(u0), v0 *sin(u0), v1*cos(u1), v1 * sin(u1)}};

	return grnd;
}

static float phillips(vec2 k) {
	float A = 4.0;
	float g = 9.81;
	vec2 waveDir = {{30.0, 0.0}};

	float k2 = k.v[0] * k.v[0] + k.v[1] * k.v[1];
	if(k2 < 0.0001) k2 = 0.0001;
	float wV = vec2Length(waveDir);
	float L = (wV*wV)/g;
	float dampingVal = 0.0001;
	float omegaK = vec2Dot(vec2Normalize(waveDir),vec2Normalize(k));

	/*
		Phillips equation
			         exp(-1/(kL)^2)
		P(k) = A * --------------- * |K . W|^2 * exp(-k^2*l^2)
						 k^4
	*/
	return A * (exp(-1.0/ (k2 * L * L))/(k2*k2)) * (omegaK*omegaK*omegaK*omegaK) * exp(-k2*L*L*dampingVal*dampingVal);
}

static void init_h0(waves_t *water) {

	float L = 2000.0;
	int offset = 0;
	for(float N = 0.0; N < water->dimension; N +=1.0) {
		for(float M = 0.0; M < water->dimension; M +=1.0) {
			/*
					 2pi*n-pi*n  2pi*m-pi*n
				k =  ----------, ----------
			             L           L
			*/
			vec2 k = {{2.0*M_PI*N/L, 2.0*M_PI*M/L}};

			/* Generate gaussian random numbers for the starting values. */
			vec4 grnd = gauss_rand();
			/* h0tilde(k) = 1/sqrt(2)*(gauss_rand(real) + gauss_rand(im))*sqrt(phillips(k)) */
			float h0 = 1/sqrt(2.0) * sqrt(phillips(k));
			vec2 kNegative = {{-k.v[0], -k.v[1]}};
			float h0Conj = 1/sqrt(2.0) * sqrt(phillips(kNegative));

			/* Calculate h0tilde */
			water->tildeh0k[offset].real = grnd.v[0]*h0; water->tildeh0k[offset].im = grnd.v[1]*h0;
			/* Calculate conjugate of h0tilde */
			water->conjTildeh0MK[offset].real = grnd.v[2]*h0Conj; water->conjTildeh0MK[offset].im = grnd.v[3]*h0Conj;
			water->conjTildeh0MK[offset] = complexConj(water->conjTildeh0MK[offset]);
			offset++;
		}
	}
}

static void calc_h0(float currentTime, waves_t *water, complexType *dx, complexType *dy, complexType *dz) {
	float L = 2000.0;
	int offset = 0;
	for(float N = 0.0; N < water->dimension; N +=1.0) {
		for(float M = 0.0; M < water->dimension; M +=1.0) {
			vec2 k = {{2.0*M_PI*N/L, 2.0*M_PI*M/L}};

			float magnitude = vec2Length(k);
			if(magnitude < 0.0001) magnitude = 0.0001;
			float w = sqrt(9.81* magnitude);

			float cosinus = cos(w+currentTime);
			float sinus = sin(w+currentTime);

			//float cosinus = cos(w);
			//float sinus = sin(w);

			/* Initialize the complex types */
			complexType expIWT = {cosinus, sinus};
			complexType invExpIWT = {cosinus, -sinus};

			dy[offset] = complexAdd(complexMult(water->tildeh0k[offset], expIWT), complexMult(water->conjTildeh0MK[offset], invExpIWT));
			dx[offset].real = 0.0; dx[offset].im = -k.v[0]/magnitude;
			dx[offset] = complexMult(dx[offset], dy[offset]);
			dz[offset].real = 0.0; dz[offset].im = -k.v[1]/magnitude;
			dz[offset] = complexMult(dz[offset], dy[offset]);

			offset++;
		}
	}
}

static void complexbit_reverse(complexType *c, unsigned int dim) {
	for(unsigned int i = 0, j = 0; i < dim; i++) {
		if(i < j) {
			complexType tmp = {c[i].real, c[i].im};
			c[i].real = c[j].real;
			c[i].im = c[j].im;
			c[j].real = tmp.real;
			c[j].im = tmp.im;
		}

		unsigned bit = ~i & (i+1);
		unsigned rev = (dim / 2) / bit;
		j ^= (dim-1) & ~(rev-1);
	}
}

static void fft(int dim, complexType *c) {
	/* Bits in fft have to be reversed to calculate the twiddle indices */
	complexbit_reverse(c, dim);
	int log2n = (int)(log(dim)/log(2));

	float c1 = -1.0;
	float c2 = 0.0;
	long l2 = 1;
	long i1;

	/* Start the fft */
	for(int l = 0; l < log2n; l++) {
		long l1 = l2;
		l2 <<= 1;
		float u1 = 1.0;
		float u2 = 0.0;
		for(int j = 0; j < l1; j++) {
			for(int i = j; i < dim; i+=l2) {
				i1 = i + l1;
				complexType temp = {u1 * c[i1].real - u2 * c[i1].im, u1 * c[i1].im + u2 * c[i1].real};
				c[i1].real = c[i].real - temp.real;
				c[i1].im = c[i].im - temp.im;
				c[i].real += temp.real;
				c[i].im += temp.im;
			}
			float z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		//c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}
	for(int i = 0; i < dim; i++) {
		c[i].real /= (float)dim;
		c[i].im /= (float)dim;
	}
}

static void fft2d(int dim, complexType *c) {
	complexType *row = malloc(dim*2*sizeof(float));
	int index = 0;
	int cpIndex = 0;

	/*First fft*/
	for(int j = 0; j < dim; j++) {
		for(int i = 0; i < dim; i++) {
			memcpy(row+i, c+index, sizeof(complexType));
			index++;
		}
		fft(dim, row);
		for(int i = 0; i < dim; i++) {
			//c[cpIndex].real = row[i].real;
			//c[cpIndex].im = row[i].im;
			memcpy(c+cpIndex, row+i, sizeof(complexType));
			cpIndex++;
		}
	}

	/* Second fft using the values from the first fft*/
	index = 0;
	for(int i = 0; i < dim; i++) {
		for(int j = 0; j < dim; j++) {
			memcpy(row+j, c+(dim*j+i), sizeof(complexType));
		}

		fft(dim, row);
		for(int j = 0; j < dim; j++) {
			memcpy(c+(dim*j+i), row+j, sizeof(complexType));
		}
	}
	free(row);
}

DLL_EXPORT void waves_init(waves_t *water, int dim) {
	/*
		Wave equation: h(k, l, t)

		N-1 N-1						 i(2pi*n*k-pi*N*k)         i(2pi*n*l-pi*N*l)
		E   E	htilde(n, m, t)*exp*(----------------- * exp * -----------------)*(1/n^2)
		n=0 m=0								N							N

	*/
	water->dimension = dim;

	/* Allocate the arrays for storing the displacement values */
	/* 2d grid for dimension * 4 for RGBA values */
	water->displacementdx = malloc(dim*dim*4*sizeof(float));
	water->displacementdy = malloc(dim*dim*4*sizeof(float));
	water->displacementdz = malloc(dim*dim*4*sizeof(float));

	water->tildeh0k = malloc(dim*dim*sizeof(complexType));
	water->conjTildeh0MK = malloc(dim*dim*sizeof(complexType));

	/* Initialize H0 (the frequency domain) since this part of the equations it only needs to be calculated once. */
	init_h0(water);
}

DLL_EXPORT void waves_generate(waves_t *water, float currentTime) {
	int dim2 = water->dimension*water->dimension;

	/* Allocate the complex type arrays for storing the delta values */
	complexType *dx = malloc(dim2*sizeof(complexType));
	complexType *dy = malloc(dim2*sizeof(complexType));
	complexType *dz = malloc(dim2*sizeof(complexType));

	/* Calculate H0 (in able to get the time domain) per frame to generate moving waves for the x, y and z directions */
	calc_h0(currentTime, water, dx, dy, dz);

	/* Calculate the 2d ffts for x, y and z directions to obtain the spatial domain*/
	fft2d(water->dimension, dy);
	fft2d(water->dimension, dx);
	fft2d(water->dimension, dz);

	/*
		Fill the displacement arrays with the scaled real values from the 2d fft
		Arrays are set to be RGBA
		RGB values are the same to produce a grayscale image
	*/
	int index = 0;
	for(int i = 0; i < dim2; i++) {
		/* Scale the values down to the correct dimension */
		//float dxh = fabs(dx[i].real/((float)dim2));
		//float dyh = fabs(dy[i].real/((float)dim2));
		//float dzh = fabs(dz[i].real/((float)dim2));

		float dxh = dx[i].real;
		float dyh = dy[i].real;
		float dzh = dz[i].real;

		//printf("dxh: %f\n", dxh);

		water->displacementdx[index] = dxh;
		water->displacementdx[index+1] = dxh;
		water->displacementdx[index+2] = dxh;
		water->displacementdx[index+3] = 1.0;

		water->displacementdy[index] = dyh;
		water->displacementdy[index+1] = dyh;
		water->displacementdy[index+2] = dyh;
		water->displacementdy[index+3] = 1.0;

		water->displacementdz[index] = dzh;
		water->displacementdz[index+1] = dzh;
		water->displacementdz[index+2] = dzh;
		water->displacementdz[index+3] = 1.0;

		index+=4;
	}

	//index = 0;
	//for(int i = 0; i < dim2; i++) {
	//    printf("displacementdx: r: %f, g: %f, b: %f, a: %f\n", water->displacementdy[index], water->displacementdy[index+1], water->displacementdy[index+2], water->displacementdy[index+3]);
	//    index+=4;
	//}

	free(dx);
	free(dy);
	free(dz);
}

DLL_EXPORT void waves_dealloc(waves_t *water) {
	free(water->displacementdx);
	water->displacementdx = NULL;
	free(water->displacementdy);
	water->displacementdy = NULL;
	free(water->displacementdz);
	water->displacementdz = NULL;
	free(water->tildeh0k);
	water->tildeh0k = NULL;
	free(water->conjTildeh0MK);
	water->conjTildeh0MK = NULL;
}