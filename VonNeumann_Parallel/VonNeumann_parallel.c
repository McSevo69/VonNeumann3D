#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIZE 262144
#define N 64
#define IT 1000

float dataIn[SIZE];
float dataOut[IT][SIZE];

int main(void) {

	srand(time(NULL));
	int i, j, k;

	for (int z = 0; z < SIZE; z++) {
		dataIn[z] = rand() / (float) RAND_MAX;
	}

	int randomIndex = rand() % 262144;
	dataIn[randomIndex] = 100000.5;

	float neighbors;
	float boundaryValue = 20.0;

	
	/* how many processors are available? */
	printf("There are %d processors\n", omp_get_num_procs());
	printf("There are max %d threads\n", omp_get_max_threads());

	clock_t start = clock();

	//very first iteration
	#pragma omp parallel private(i,j,k) shared(dataOut, dataIn)
		{
			#pragma omp for nowait
			for (i = 0; i < N; i++) {
				for (j = 0; j < N; j++) {
					for (k = 0; k < N; k++) {

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
		}		

	//iterations 1 - N (dataIn isn't needed anymore)
	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		printf("Iteration %d\n" , it_cnt);
	
		#pragma omp parallel private(i,j,k) shared(dataOut)
		{
			#pragma omp for nowait
			for (i = 0; i < N; i++) {
				for (j = 0; j < N; j++) {
					for (k = 0; k < N; k++) {

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
	}

	clock_t end = clock();
	double timeSpent = (end-start)/(double)CLOCKS_PER_SEC/omp_get_num_procs();
	printf("time: %.2f sec\n",timeSpent);
	printf("results are stored.\n");

	FILE* results;
	results = fopen("results_par_1000.csv","a");

	for (int x = 1; x < IT; x++) {

		for(int y = 0; y < SIZE; ++y){
			fprintf(results, "%f,", dataOut[x][y]);
		}

		fprintf(results, "\n");

	}

	fclose (results);
	printf("done.");

	
	return 0;
}

