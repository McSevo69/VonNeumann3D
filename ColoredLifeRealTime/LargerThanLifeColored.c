#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include <omp.h>

#define SHAPE_SIZE 1
#define SIZE 262144
#define N 512
#define IT 2200
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

//0 - dead
//1 - fit
//2 - weak
//3 - overfit
void golAlgorithm(int *dataIn, int *dataOut, int b1, int b2, int w, int f1, int f2, int of, int oof, int radius) {

	int neighbors;
	//int cellIndex;
	int cellRowUp;
	int cellRowDown;
	int cellColLeft;
	int cellColRight;

	int rightOF;
	int leftUF; //left underflow
	int bottomOF;
	int topUF; 

	for (int j = 0; j < N; j++) {

		for (int k = 0; k < N; k++) {

			neighbors = 0;
			//cellIndex = j*N+k;
			topUF = 0;
			bottomOF = 0;			
	
			//diagonal squares
			for (int ir = 0; ir < radius; ir++) {
					
				//move up
				cellRowUp = (j-ir-1 < 0) ? (N-(++topUF)) * N : (j-ir-1) * N;
	
				//move down
				cellRowDown = (j+ir+1 > N) ? (bottomOF++) * N : (j+ir+1) * N;

				rightOF = 0;
				leftUF = 0;
	
				for (int jr=0; jr<radius; jr++) {

					//move left
					cellColLeft = ((k-jr-1) < 0) ? N - (++leftUF) : (k-jr-1);
			
					//move right
					cellColRight = ((k+jr+1) > N) ? (rightOF++) : (k+jr+1);
				
					if (dataIn[cellRowUp+cellColRight]) neighbors++;
					if (dataIn[cellRowUp+cellColLeft]) neighbors++;
					if (dataIn[cellRowDown+cellColRight]) neighbors++;
					if (dataIn[cellRowDown+cellColLeft]) neighbors++;

				}
			}

			//'cross' around f(x,y)

			topUF = 0;
			bottomOF = 0;			

			//above-below
			for (int ic = 0; ic < radius; ic++) {
					
				//move up
				cellRowUp = (j-ic-1 < 0) ? (N-(++topUF)) * N : (j-ic-1) * N;
	
				//move down
				cellRowDown = (j+ic+1 > N) ? (bottomOF++) * N : (j+ic+1) * N;
				
				if (dataIn[cellRowUp+k]) neighbors++;
				if (dataIn[cellRowDown+k]) neighbors++;

			}

			leftUF = 0;
			rightOF = 0;
			
			//left-right
			for (int jc=0; jc<radius; jc++) {

					//move left
					cellColLeft = ((k-jc-1) < 0) ? N - (++leftUF) : (k-jc-1);
			
					//move right
					cellColRight = ((k+jc+1) > N) ? (rightOF++) : (k+jc+1);
				
					if (dataIn[j*N+cellColRight]) neighbors++;
					if (dataIn[j*N+cellColLeft]) neighbors++;

			}

			//rules for surviving
			switch(dataIn[j*N+k]) {
				case 0: dataOut[j*N+k] = ((neighbors>=b1) && (neighbors<=b2)) ? 1 : 0; break;
				default:
					if (neighbors >= w && neighbors < f1) { //weak
						dataOut[j*N+k] = 2;
						break;
					} else if (neighbors >= f1 && neighbors <= f2) { //fit
						dataOut[j*N+k] = 1;
						break;
					} else if (neighbors > f2 && neighbors <= of) { //overfit
						dataOut[j*N+k] = 3;
						break;
					} else if (neighbors > of && neighbors <= oof) { //overfit
						dataOut[j*N+k] = 4;
						break;
					} else {
						dataOut[j*N+k] = 0;
						break;
					}
			}				

		}
	}

}


int main(int argc, char *argv[]) {

	int radius=10; //radius of the neighborhood
	int maxNeighbors = (2*radius+1)*(2*radius+1)-1;
	int b1 = maxNeighbors*0.28; //birth 1
	int b2 = maxNeighbors*0.375; //birth 2

	int w = maxNeighbors*0.27;      //weak in range [w,f1[
	int f1 = maxNeighbors*0.33;		//fit in range [f1,f2]
	int f2 = maxNeighbors*0.40;
	int of = maxNeighbors*0.44;    //overfit in range ]f2,of]
	int oof = maxNeighbors*0.482;	//overfit in range ]of,oof]

	float alive = 0.692; //must be in range [0,1]

	int * dataIn = calloc(SIZE, sizeof(int));
	int ** dataOut = calloc(IT*SIZE, sizeof(int*));
	for (int i=0; i<IT; i++) dataOut[i] = calloc(SIZE, sizeof(int));

	init(dataIn, N, SIZE*alive);

	/* how many processors are available? */
	//printf("There are %d processors\n", omp_get_num_procs());
	//printf("There are max %d threads\n", omp_get_max_threads());

	//first iteration
	//printf("Iteration 1\n");
	golAlgorithm(dataIn,dataOut[0], b1, b2, w, f1, f2, of, oof, radius);

	//buffering
	//for (b1 = 1; b1 < buff+1; b1++) golAlgorithm(dataOut[b1-1],dataOut[b1], b1, b2, w, f1, f2, of, radius);
	
	SDL_Window* Main_Window;
	SDL_Renderer* Main_Renderer;
	SDL_Surface* Loading_Surf;
	SDL_Texture* Background_Tx;
	SDL_Texture* WeakCell;
	SDL_Texture* FitCell;
	SDL_Texture* OverfitCell;
	SDL_Texture* OverOverfitCell;

	/* Rectangles for drawing which will specify source (inside the texture)
	and target (on the screen) for rendering our textures. */
	SDL_Rect SrcR;
	SDL_Rect DestR;

	SrcR.x = 0;
	SrcR.y = 0;
	SrcR.w = SHAPE_SIZE;
	SrcR.h = SHAPE_SIZE;

	DestR.w = SHAPE_SIZE;
	DestR.h = SHAPE_SIZE;


	/* Before we can render anything, we need a window and a renderer */
	Main_Window = SDL_CreateWindow("Larger Life Colored",
	SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_WIDTH, 0);
	Main_Renderer = SDL_CreateRenderer(Main_Window, -1, SDL_RENDERER_ACCELERATED);

	/* The loading of the background texture. Since SDL_LoadBMP() returns
	a surface, we convert it to a texture afterwards for fast accelerated
	blitting. */
	Loading_Surf = SDL_LoadBMP("background.bmp");
	Background_Tx = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf); /* we got the texture now -> free surface */

	/* Load an additional texture */
	Loading_Surf = SDL_LoadBMP("weak.bmp");
	WeakCell = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	/* Load an additional texture */
	Loading_Surf = SDL_LoadBMP("fit.bmp");
	FitCell = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	/* Load an additional texture */
	Loading_Surf = SDL_LoadBMP("overfit.bmp");
	OverfitCell = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	/* Load an additional texture */
	Loading_Surf = SDL_LoadBMP("overoverfit.bmp");
	OverOverfitCell = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

	/* now 'plotting'*/
	int i;
	int currentCell;

	//#pragma omp parallel
	//{

		for (i = 1; i < IT; ++i) {
				
			//#pragma omp barrier

				//if (omp_get_thread_num()==0) {	

					/* render background, whereas NULL for source and destination
					rectangles just means "use the default" */
					SDL_RenderCopy(Main_Renderer, Background_Tx, NULL, NULL);

					/* render the current animation step of our shape */
					//SDL_RenderCopy(Main_Renderer, BlueShapes, &SrcR, &DestR);

					for (int j=0; j<N; j++) {
						for (int k=0; k<N; k++) {
							currentCell = dataOut[i-1][j*N+k];
							if (currentCell == 2) { //weak
								DestR.x = SHAPE_SIZE*2*(k) / 2;
								DestR.y = SHAPE_SIZE*2*(j) / 2;
								SDL_UpdateTexture(WeakCell, NULL, NULL, Loading_Surf->pitch);
								SDL_RenderCopy(Main_Renderer, WeakCell, &SrcR, &DestR);
							} else if (currentCell == 1) { //fit
								DestR.x = SHAPE_SIZE*2*(k) / 2;
								DestR.y = SHAPE_SIZE*2*(j) / 2;
								SDL_UpdateTexture(FitCell, NULL, NULL, Loading_Surf->pitch);
								SDL_RenderCopy(Main_Renderer, FitCell, &SrcR, &DestR);
							} else if (currentCell == 3) { //overfit
								DestR.x = SHAPE_SIZE*2*(k) / 2;
								DestR.y = SHAPE_SIZE*2*(j) / 2;
								SDL_UpdateTexture(OverfitCell, NULL, NULL, Loading_Surf->pitch);
								SDL_RenderCopy(Main_Renderer, OverfitCell, &SrcR, &DestR);
							} else if (currentCell == 4) { //overoverfit
								DestR.x = SHAPE_SIZE*2*(k) / 2;
								DestR.y = SHAPE_SIZE*2*(j) / 2;
								SDL_UpdateTexture(OverfitCell, NULL, NULL, Loading_Surf->pitch);
								SDL_RenderCopy(Main_Renderer, OverOverfitCell, &SrcR, &DestR);
							}
						}
					}

					SDL_RenderPresent(Main_Renderer);
				//} else if (omp_get_thread_num()==1) {

					int cnt = 0;
					while(++cnt < 2) {
						SDL_Event event;
						SDL_PollEvent( &event );
						SDL_Delay(1);
					}

					clock_t start = clock();
					printf("Iteration %d\n" , i);
					golAlgorithm(dataOut[i-1],dataOut[i], b1, b2, w, f1, f2, of, oof, radius);

					clock_t end = clock();
					double timeSpent = (end-start)/(double)CLOCKS_PER_SEC;
					printf("time: %.2f sec\n",timeSpent);
			//}
		}


	//}

	SDL_DestroyTexture(WeakCell);
	SDL_DestroyTexture(FitCell);
	SDL_DestroyTexture(OverfitCell);
	SDL_DestroyTexture(OverOverfitCell);
	SDL_DestroyTexture(Background_Tx);
	SDL_DestroyRenderer(Main_Renderer);
	SDL_DestroyWindow(Main_Window);
	SDL_Quit();

	free(dataIn);
	for (int i=0; i<IT; i++) free(dataOut[i]);
	free(dataOut);

	return 0;
}
