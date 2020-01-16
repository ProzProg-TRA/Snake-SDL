#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>

#define GRID_RES_X 16
#define GRID_RES_Y 9    
#define GRID_CELL_SIZE 100

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Texture* bitmapTex;
SDL_Surface* bitmapSurface;


void draw()
{
    SDL_RenderClear(renderer);
    SDL_Rect rect;

    for (int y = 0; y < GRID_RES_Y; y++)
    {
        for (int x = 0; x < GRID_RES_X; x++)
        {
            if ((x+y)%2)
            {
                rect.x = x * GRID_CELL_SIZE;
                rect.y = y * GRID_CELL_SIZE;
                rect.w = rect.h = GRID_CELL_SIZE;
                SDL_RenderCopy(renderer, bitmapTex, NULL, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void step()
{
    
}

int main()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	}

	window = SDL_CreateWindow("Snake", // creates a window 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									GRID_CELL_SIZE * GRID_RES_X, 
                                    GRID_CELL_SIZE * GRID_RES_Y, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bitmapSurface = SDL_LoadBMP("res/red.bmp");
    bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);

    int should_close = 0;

    //main loop
    while (should_close == 0)
    {
        step();
        draw();
    }
}