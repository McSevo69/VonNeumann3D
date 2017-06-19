#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SHAPE_SIZE 1
#define SIZE 861184
#define N 928
#define IT 2000
#define WINDOW_WIDTH 928

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

void golAlgorithm(int *dataIn, int *dataOut, int s1, int s2, int b1, int b2, int radius) {

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
			leftUF = 0;
			bottomOF = 0;
			rightOF = 0;
	
			//diagonal squares
			for (int ir = 0; ir < radius; ir++) {
					
				//move up
				cellRowUp = (j-ir-1 < 0) ? (N-(++topUF)) * N : (j-ir-1) * N;
	
				//move down
				cellRowDown = (j+ir+1 > N) ? (bottomOF++) * N : (j+ir+1) * N;
	
				for (int jr=0; jr<radius; jr++) {

					//move left
					cellColLeft = ((k-jr-1) < 0) ? N - (++leftUF) : (k-jr-1);
			
					//move right
					cellColRight = ((k+jr+1) > N) ? (rightOF++) : (k+jr+1);
				
					neighbors += dataIn[cellRowUp+cellColRight];
					neighbors += dataIn[cellRowUp+cellColLeft];
					neighbors += dataIn[cellRowDown+cellColRight];
					neighbors += dataIn[cellRowDown+cellColLeft];

				}
			}

			//'cross' around f(x,y)
			leftUF = 0;
			rightOF = 0;
			topUF = 0;
			bottomOF = 0;

			//above-below
			for (int ic = 0; ic < radius; ic++) {
					
				//move up
				cellRowUp = (j-ic-1 < 0) ? (N-(++topUF)) * N : (j-ic-1) * N;
	
				//move down
				cellRowDown = (j+ic+1 > N) ? (bottomOF++) * N : (j+ic+1) * N;
				
				neighbors += dataIn[cellRowUp+k];
				neighbors += dataIn[cellRowDown+k];

			}
			
			//left-right
			for (int jc=0; jc<radius; jc++) {

					//move left
					cellColLeft = ((k-jc-1) < 0) ? N - (++leftUF) : (k-jc-1);
			
					//move right
					cellColRight = ((k+jc+1) > N) ? (rightOF++) : (k+jc+1);
				
					neighbors += dataIn[j*N+cellColRight];
					neighbors += dataIn[j*N+cellColLeft];

			}

			//rules for surviving
			switch(dataIn[j*N+k]) {
				case 0: dataOut[j*N+k] = ((neighbors>=b1) && (neighbors<=b2)) ? 1 : 0; break;
				case 1:
					//loneliness or overpopulation
					dataOut[j*N+k]= ((neighbors>=s1) && (neighbors<=s2)) ? 1 : 0; break;
			}				

		}
	}

}


int main(int argc, char *argv[])
{
	int radius=20; //radius of the neighborhood
	int maxNeighbors = (2*radius+1)*(2*radius+1)-1;
	int s1 = maxNeighbors*0.28; //survival 1
	int s2 = maxNeighbors*0.48; //survival 2
	int b1 = maxNeighbors*0.28; //birth 1
	int b2 = maxNeighbors*0.375; //birth 2
	float alive = 0.662; //must be in range [0,1]

	int * dataIn = calloc(SIZE, sizeof(int));
	int ** dataOut = calloc(IT*SIZE, sizeof(int*));
	for (int i=0; i<IT; i++) dataOut[i] = calloc(SIZE, sizeof(int));

	init(dataIn, N, SIZE*alive);

	clock_t start = clock();

	//first iteration
	//printf("Iteration 1\n");
	golAlgorithm(dataIn,dataOut[0], s1, s2, b1, b2, radius);

	for (int it_cnt = 1; it_cnt < IT; it_cnt++) {
		//printf("Iteration %d\n" , it_cnt+1);
		golAlgorithm(dataOut[it_cnt-1],dataOut[it_cnt], s1, s2, b1, b2, radius);
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

  DestR.w = SHAPE_SIZE;
  DestR.h = SHAPE_SIZE;


  /* Before we can render anything, we need a window and a renderer */
  Main_Window = SDL_CreateWindow("Game Of Life",
  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_WIDTH, 0);
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
      int cnt = 0;
      while(++cnt < 100) {
          SDL_Event event;
          SDL_PollEvent( &event );
          SDL_Delay(1);
      }
    
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

