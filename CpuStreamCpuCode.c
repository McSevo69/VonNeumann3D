#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#define SIZE 262144
#define N 64
#define IT 10

float dataIn[SIZE];
float dataOut[IT][SIZE];

int main(void) {

	for (int i = 0; i < SIZE; i++) {
		dataIn[i] = i;
	}

	printf("Running on DFE.\n");
	CpuStream(SIZE, N, dataIn, &dataOut[0][0]);

	for (int i = 1; i < IT; i++) {
		CpuStream(SIZE, N, &dataOut[i-1][0], &dataOut[i][0]);
	}

	// TODO Use result data
	for (int j = 1; j < IT; j++) {
		printf("Iteration %d\n" , j);
		for(int i = 0; i < SIZE; ++i){
			printf("dataOut[%d] = %f\n", i, dataOut[j][i]);
		}
	}

	printf("Done.\n");
	return 0;
}
