#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SDL2/SDL.h"

#define SIZE 16384
#define N 128
#define IT 1000
#define WINDOW_WIDTH 512

//initialization
void init(int *a, int n, int alife) {

	int randomIndex;
	srand(time(NULL));

	// alive ones
	for (int i=0; i<alife; i++) {
		randomIndex = rand() % (n*n);
		a[randomIndex] = 1;
	}

}

void golAlgorithm(int *dataIn, int *dataOut) {

	int neighbors;

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
				case 0: if (neighbors==3) dataOut[j*N+k]=1; break;
				case 1:
					//loneliness
					if (neighbors < 2) dataOut[j*N+k]=0; break;
					//overpopulation
					if (neighbors > 3) dataOut[j*N+k]=0; break;
			}				

		}
	}

}

void quit(int rc) {
	
	SDL_Quit();
	exit(rc);
}

void drawResults(SDL_Renderer *rend, int *dataToPlot) {

	for (int i=0; i<N; i++) {
		for (int j=0; j<N; j++) {
			if (dataToPlot[i*N+j] == 1) {
				for (int a=0; a<4; a++) {
					for (int b=0; b<4; b++) {
						SDL_RenderDrawPoint(rend, 4*i-a, 4*j-b);
					}
				}
				//SDL_RenderDrawPoint(rend, i, j);
			}
	
		}
	}
		
	SDL_RenderPresent(rend);


}


int main( int argc, char* args[] ) {

	//SDL		
	SDL_Renderer *renderer;
	SDL_Window *window;

	/* Create the window and renderer */
    window = SDL_CreateWindow("Game Of Life",
							SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
							N*4, N*4,
							SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);


	int * dataIn = malloc(SIZE*sizeof(int));
	int ** dataOut = calloc(IT*SIZE, sizeof(int*));
	for (int i=0; i<IT; i++) dataOut[i] = calloc(SIZE, sizeof(int));

	init(dataIn, N, (N*N-N/4));

	clock_t start = clock();

	//first iteration
	printf("Iteration 1\n");
	golAlgorithm(dataIn,dataOut[0]);

	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		printf("Iteration %d\n" , it_cnt+1);
		golAlgorithm(dataOut[it_cnt-1],dataOut[it_cnt]);
	}

	clock_t end = clock();
	double timeSpent = (end-start)/(double)CLOCKS_PER_SEC;
	printf("time: %.2f sec\n",timeSpent);
	printf("results are stored.\n");

	FILE* results;
	results = fopen("results_seq_new.csv","a");

	for (int j = 1; j < IT; j++) {

		for(int i = 0; i < SIZE; ++i){
			fprintf(results, "%d,", dataOut[j][i]);
		}

		fprintf(results, "\n");

	}

	fclose (results);
	printf("done.");
	
	free(dataIn);


	for (int j = 0; j < IT; j++) {
		drawResults(renderer, dataOut[j]);
		free(dataOut[j]);
		SDL_Delay(400);
	}

	free(dataOut);

	//Quit SDL
	SDL_DestroyRenderer(renderer);
 	SDL_DestroyWindow(window);

	quit(0);

}

