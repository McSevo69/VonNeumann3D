#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SHAPE_SIZE 8
#define SIZE 4096
#define N 64
#define IT 250
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

	//glider
	//a[2] = 1;
	//a[N] = 1;
	//a[N+2] = 1;
	//a[2*N+1] = 1;
	//a[2*N+2] = 1;

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
				case 0: if (neighbors==3) {
							dataOut[j*N+k]=1; 
							break;
						} else {
							dataOut[j*N+k]=0;
							break;
						}
				case 1:
					//loneliness
					if (( neighbors < 2) || (neighbors > 3)) {
						dataOut[j*N+k]=0; 
						break;
					} else {
						dataOut[j*N+k]=1;
						break;
					}
			}				

		}
	}

}


int main(int argc, char *argv[])
{

	int * dataIn = calloc(SIZE, sizeof(int));
	int ** dataOut = calloc(IT*SIZE, sizeof(int*));
	for (int i=0; i<IT; i++) dataOut[i] = calloc(SIZE, sizeof(int));

	init(dataIn, N, SIZE-N/8);

	clock_t start = clock();

	//first iteration
	//printf("Iteration 1\n");
	golAlgorithm(dataIn,dataOut[0]);

	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		//printf("Iteration %d\n" , it_cnt+1);
		golAlgorithm(dataOut[it_cnt-1],dataOut[it_cnt]);
	}

	clock_t end = clock();
	double timeSpent = (end-start)/(double)CLOCKS_PER_SEC;
	printf("time: %.2f sec\n",timeSpent);

  SDL_Window* Main_Window;
  SDL_Renderer* Main_Renderer;
  SDL_Surface* Loading_Surf;
  SDL_Texture* Background_Tx;
  SDL_Texture* WhiteCell;

  /* Rectangles for drawing which will specify source (inside the texture)
  and target (on the screen) for rendering our textures. */
  SDL_Rect SrcR;
  SDL_Rect DestR;

  SrcR.x = 0;
  SrcR.y = 0;
  SrcR.w = SHAPE_SIZE;
  SrcR.h = SHAPE_SIZE;

  DestR.x = SHAPE_SIZE / 2 - SHAPE_SIZE / 2;
  DestR.y = SHAPE_SIZE / 2 - SHAPE_SIZE / 2;
  DestR.w = SHAPE_SIZE;
  DestR.h = SHAPE_SIZE;


  /* Before we can render anything, we need a window and a renderer */
  Main_Window = SDL_CreateWindow("Game Of Life",
  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, 0);
  Main_Renderer = SDL_CreateRenderer(Main_Window, -1, SDL_RENDERER_ACCELERATED);

  /* The loading of the background texture. Since SDL_LoadBMP() returns
  a surface, we convert it to a texture afterwards for fast accelerated
  blitting. */
  Loading_Surf = SDL_LoadBMP("background.bmp");
  Background_Tx = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
  SDL_FreeSurface(Loading_Surf); /* we got the texture now -> free surface */

  /* Load an additional texture */
  Loading_Surf = SDL_LoadBMP("white.bmp");
  WhiteCell = SDL_CreateTextureFromSurface(Main_Renderer, Loading_Surf);
  SDL_FreeSurface(Loading_Surf);

  /* now onto the fun part.
  This will render a rotating selection of the blue shapes
  in the middle of the screen */
  int i;
  //int n;
  for (i = 0; i < IT; ++i) {

      /* render background, whereas NULL for source and destination
      rectangles just means "use the default" */
      SDL_RenderCopy(Main_Renderer, Background_Tx, NULL, NULL);

      /* render the current animation step of our shape */
      //SDL_RenderCopy(Main_Renderer, BlueShapes, &SrcR, &DestR);

	  for (int j=0; j<N; j++) {
		for (int k=0; k<N; k++) {
			if (dataOut[i][j*N+k]==1) {
      			DestR.x = SHAPE_SIZE*2*(k) / 2;
  	  			DestR.y = SHAPE_SIZE*2*(j) / 2;
				SDL_UpdateTexture(WhiteCell, NULL, NULL, Loading_Surf->pitch);
		    	SDL_RenderCopy(Main_Renderer, WhiteCell, &SrcR, &DestR);
			}
		}
	  }

      SDL_RenderPresent(Main_Renderer);
      SDL_Delay(300);
    
  }


  /* The renderer works pretty much like a big canvas:
  when you RenderCopy() you are adding paint, each time adding it
  on top.
  You can change how it blends with the stuff that
  the new data goes over.
  When your 'picture' is complete, you show it
  by using SDL_RenderPresent(). */

  /* SDL 1.2 hint: If you're stuck on the whole renderer idea coming
  from 1.2 surfaces and blitting, think of the renderer as your
  main surface, and SDL_RenderCopy() as the blit function to that main
  surface, with SDL_RenderPresent() as the old SDL_Flip() function.*/

  SDL_DestroyTexture(WhiteCell);
  SDL_DestroyTexture(Background_Tx);
  SDL_DestroyRenderer(Main_Renderer);
  SDL_DestroyWindow(Main_Window);
  SDL_Quit();


	free(dataIn);
	for (int i=0; i<IT; i++) free(dataOut[i]);
	free(dataOut);


  return 0;
}

