#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

#define SIZE 2097152
#define N 128
#define IT 1000

float dataIn[SIZE];
float dataOut[IT][SIZE];

int main(void) {

	srand(time(NULL));

	//random initialization
	for (int i = 0; i < SIZE; i++) {
		dataIn[i] = rand() / (float) RAND_MAX;
	}

	int randomIndex = rand() % 262144;
	dataIn[randomIndex] = 10000000.5;

	printf("Running on DFE.\n");
	clock_t start = clock();
	CpuStream(SIZE, N, dataIn, &dataOut[0][0]);

	for (int i = 1; i < IT; i++) {
		printf("Iteration %d\n" , i);
		CpuStream(SIZE, N, &dataOut[i-1][0], &dataOut[i][0]);
	}

	clock_t end = clock();
	double timeSpent = (end-start)/(double)CLOCKS_PER_SEC;
	printf("time: %.2f sec\n",timeSpent);
	printf("results are stored.\n");

	FILE* results;
	results = fopen("results_new_1000.csv","a");

	for (int j = 1; j < IT; j++) {
		for(int i = 0; i < SIZE; ++i){
			fprintf(results, "%f,", dataOut[j][i]);
		}
		fprintf(results, "\n");
	}

	fclose (results);

	printf("Done.\n");
	return 0;
}
