#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#define SIZE 262144
#define N 64
#define IT 10

float dataIn[SIZE];
float dataOut[IT][SIZE];

int main(void) {

	srand(time(NULL));

	for (int i = 0; i < SIZE; i++) {
		dataIn[i] = rand() / (float) RAND_MAX;
	}

	int randomIndex = rand() % 262144;
	dataIn[randomIndex] = 100000.5;

	printf("Running on DFE.\n");
	CpuStream(SIZE, N, dataIn, &dataOut[0][0]);

	FILE* results;
	results = fopen("results.csv","a");

	for (int i = 1; i < IT; i++) {
		CpuStream(SIZE, N, &dataOut[i-1][0], &dataOut[i][0]);
	}

	// TODO Use result data
	for (int j = 1; j < IT; j++) {
		printf("Iteration %d\n" , j);

		for(int i = 0; i < SIZE; ++i){
			fprintf(results, "%f,", dataOut[j][i]);
		}

		fprintf(results, "\n");

	}

	fclose (results);

	printf("Done.\n");
	return 0;
}

