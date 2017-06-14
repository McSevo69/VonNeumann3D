#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 262144
#define N 64
#define IT 1000

int main(void) {

	float * dataIn = calloc(N*N*N, sizeof(float));
	float ** dataOut = (float **) malloc(sizeof(float[IT][N*N*N]));
	for (int i=0; i<IT; i++) dataOut[i] = (float*) malloc(N*N*N*sizeof(float));

	srand(time(NULL));	

	//random initialization
	for (int i = 0; i < SIZE; i++) {
		dataIn[i] = rand() / (float) RAND_MAX;
	}

	int randomIndex = rand() % 262144;
	dataIn[randomIndex] = 100000.5;

	float neighbors;
	float boundaryValue = 20.0;

	clock_t start = clock();

	//very first iteration
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {

				//left boundary
				neighbors = (k==0) ? boundaryValue : dataIn[i*N*N+j*N+k-1];

				//right boundary
				neighbors += (k==N) ? boundaryValue : dataIn[i*N*N+j*N+k+1];
			
				//upper boundary
				neighbors += (j==0) ? boundaryValue : dataIn[i*N*N+j*N+k-N];

				//lower boundary
				neighbors += (j==N) ? boundaryValue : dataIn[i*N*N+j*N+k+N];
					
				//above boundary
				neighbors += (i==0) ? boundaryValue : dataIn[i*N*N+j*N+k-N*N];
			
				//below boundary
				neighbors += (i==N) ? boundaryValue : dataIn[i*N*N+j*N+k+N*N];
		
				dataOut[0][i*N*N+j*N+k] = neighbors/6;

			}
		}
	}

	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		printf("Iteration %d\n" , it_cnt);

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++) {

						//left boundary
						neighbors = (k==0) ? boundaryValue : dataOut[it_cnt-1][i*N*N+j*N+k-1];

						//right boundary
						neighbors += (k==N) ? boundaryValue : dataOut[it_cnt-1][i*N*N+j*N+k+1];
			
						//upper boundary
						neighbors += (j==0) ? boundaryValue : dataOut[it_cnt-1][i*N*N+j*N+k-N];

						//lower boundary
						neighbors += (j==N) ? boundaryValue : dataOut[it_cnt-1][i*N*N+j*N+k+N];
					
						//above boundary
						neighbors += (i==0) ? boundaryValue : dataOut[it_cnt-1][i*N*N+j*N+k-N*N];
			
						//below boundary
						neighbors += (i==N) ? boundaryValue : dataOut[it_cnt-1][i*N*N+j*N+k+N*N];
		
						dataOut[it_cnt][i*N*N+j*N+k] = neighbors/6;

				}
			}
		}
	}

	clock_t end = clock();
	double timeSpent = (end-start)/(double)CLOCKS_PER_SEC;
	printf("time: %.2f sec\n",timeSpent);
	printf("results are stored.\n");

	FILE* results;
	results = fopen("results_seq_new.csv","a");

	for (int j = 1; j < IT; j++) {

		for(int i = 0; i < SIZE; ++i){
			fprintf(results, "%f,", dataOut[j][i]);
		}

		fprintf(results, "\n");

	}

	fclose (results);

	free(dataIn);
	for (int i=0; i<IT; i++) free(dataOut[i]);
	free(dataOut);

	printf("done.");

	
	return 0;
}

