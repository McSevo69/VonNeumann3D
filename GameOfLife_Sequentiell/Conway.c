#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 16384
#define N 128
#define IT 1000

int dataOut[IT][SIZE];

//initialization
void init(int *a, int n, int alife) {

	int randomIndex;
	srand(time(NULL));

	//dead ones
	for (int j = 0; j < n*n; j++) {
		a[j] = 0;
	}

	// alife ones
	for (int i=0; i<alife; i++) {
		randomIndex = rand() % (n*n);
		a[randomIndex] = 1;
	}

}


int main(void) {

	int * dataIn;

	dataIn = calloc(N*N, sizeof(int));

	init(dataIn, N, (N*N/4));

	int neighbors;

	clock_t start = clock();

	//very first iteration
	for (int j = 0; j < N; j++) {
		for (int k = 0; k < N; k++) {

			//left boundary - left neighbor
			neighbors = (k==0) ? 0 : dataIn[j*N+k-1];

			//right boundary - right neighbor
			neighbors += (k==N) ? 0 : dataIn[j*N+k+1];
			
			//upper boundary - top neighbor
			neighbors += (j==0) ? 0 : dataIn[j*N+k-N];

			//lower boundary - bottom neighbor
			neighbors += (j==N) ? 0 : dataIn[j*N+k+N];

			//top left neighbor
			neighbors += (k==0 || j==0) ? 0 : dataIn[j*N+k-N-1];

			//top right neighbor
			neighbors += (k==N || j==0) ? 0 : dataIn[j*N+k-N+1];

			//bottom left neighbor
			neighbors += (k==0 || j==N) ? 0 : dataIn[j*N+k+N-1];

			//bottom right neighbor
			neighbors += (k==N || j==N) ? 0 : dataIn[j*N+k+N+1];
					
	
			//rules for surviving
			switch(dataIn[j*N+k]) {
				case 0: if (neighbors==3) dataOut[0][j*N+k]=1; break;
				case 1:
					//loneliness
					if (neighbors < 2) dataOut[0][j*N+k]=0; break;
					//overpopulation
					if (neighbors > 3) dataOut[0][j*N+k]=0; break;
			}				

		}
	}
	

	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		printf("Iteration %d\n" , it_cnt);

		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {

				//left boundary - left neighbor
				neighbors = (k==0) ? 0 : dataOut[it_cnt-1][j*N+k-1];

				//right boundary - right neighbor
				neighbors += (k==N) ? 0 : dataOut[it_cnt-1][j*N+k+1];
			
				//upper boundary - top neighbor
				neighbors += (j==0) ? 0 : dataOut[it_cnt-1][j*N+k-N];

				//lower boundary - bottom neighbor
				neighbors += (j==N) ? 0 : dataOut[it_cnt-1][j*N+k+N];

				//top left neighbor
				neighbors += (k==0 || j==0) ? 0 : dataOut[it_cnt-1][j*N+k-N-1];

				//top right neighbor
				neighbors += (k==N || j==0) ? 0 : dataOut[it_cnt-1][j*N+k-N+1];

				//bottom left neighbor
				neighbors += (k==0 || j==N) ? 0 : dataOut[it_cnt-1][j*N+k+N-1];

				//bottom right neighbor
				neighbors += (k==N || j==N) ? 0 : dataOut[it_cnt-1][j*N+k+N+1];
				
				//rules for surviving					
				switch(dataOut[it_cnt-1][j*N+k]) {
					case 0: if (neighbors==3) dataOut[it_cnt][j*N+k]=1; break;
					case 1:
					//loneliness
					if (neighbors < 2) dataOut[it_cnt][j*N+k]=0; break;
					//overpopulation
					if (neighbors > 3) dataOut[it_cnt][j*N+k]=0; break;
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

	// TODO Use result data
	for (int j = 1; j < IT; j++) {

		for(int i = 0; i < SIZE; ++i){
			fprintf(results, "%d,", dataOut[j][i]);
		}

		fprintf(results, "\n");

	}

	fclose (results);
	printf("done.");

	
	return 0;
}

