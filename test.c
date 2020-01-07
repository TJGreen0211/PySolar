#include <stdio.h>
#include <stdlib.h>
#include "water.h"

#define DLL_EXPORT __declspec(dllexport)

//cl /W3 /LD /TC test.c

DLL_EXPORT int print_msg()
{
	printf("Hello World\n");
	return 0;
}

DLL_EXPORT int add(int a, int b)
{
	return a + b;
}

DLL_EXPORT double sv4(double x, double y, double z)
{
	return 10.0;
}

DLL_EXPORT void testWaves(int dim) {
	struct waves ocean;
	initializeWaves(&ocean, dim);
	generateWaves(&ocean, 1.0);
}

DLL_EXPORT int *test_function(int num_numbers, int *numbers) {
	struct waves ocean;
	initializeWaves(&ocean, 256);
	generateWaves(&ocean, 1.0);

	int i;
	int *i_arr = malloc(num_numbers*sizeof(int));
	int sum = 0;
	for (i = 0; i < num_numbers; i++){
		sum += numbers[i];
		i_arr[i] = sum;
	}
	printf("Funtion completed\n");
	return i_arr;
}