#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 262144
#define N 64
#define IT 1000

float dataIn[SIZE];
float dataOut[IT][SIZE];

int main(void) {

	srand(time(NULL));

	for (int i = 0; i < SIZE; i++) {
		dataIn[i] = rand() / (float) RAND_MAX;
	}

	int randomIndex = rand() % 262144;
	dataIn[randomIndex] = 100000.5;

	float neighbors;
	float boundaryValue = 20.0;

	clock_t start = clock();

	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		printf("Iteration %d\n" , j);

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++) {

					//left boundary
					if (k==0) {
						neighbors = boundaryValue;
					} else {
						neighbors = dataOut[it_cnt-1][i*N*N+j*N+k-1];
					}

					//right boundary
					if (k==N) {
						neighbors += boundaryValue;
					} else {
						neighbors += dataOut[it_cnt-1][i*N*N+j*N+k+1];
					}


					//upper boundary
					if (j==0) {
						neighbors += boundaryValue;
					} else {
						neighbors += dataOut[it_cnt-1][i*N*N+j*N+k-N];
					}

					//lower boundary
					if (j==N) {
						neighbors += boundaryValue;
					} else {
						neighbors += dataOut[it_cnt-1][i*N*N+j*N+k+N];
					}

					//above boundary
					if (i==0) {
						neighbors += boundaryValue;
					} else {
						neighbors += dataOut[it_cnt-1][i*N*N+j*N+k-N*N];
					}

					//below boundary
					if (i==N) {
						neighbors += boundaryValue;
					} else {
						neighbors += dataOut[it_cnt-1][i*N*N+j*N+k+N*N];
					}

					dataOut[it_cnt][i*N*N+j*N+k] = neighbors/6;

				}
			}
		}
	}

	clock_t end = clock();
	double timeSpent = (end-start)/(double)CLOCKS_PER_SEC;

	FILE* results;
	results = fopen("results_seq_1000.csv","a");

	// TODO Use result data
	for (int j = 1; j < IT; j++) {

		for(int i = 0; i < SIZE; ++i){
			fprintf(results, "%f,", dataOut[j][i]);
		}

		fprintf(results, "\n");

	}

	fclose (results);

	printf("Done. time: %.2f sec\n",timeSpent);
	return 0;
}

