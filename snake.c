#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>

#define GRID_RES_X 16
#define GRID_RES_Y 9    
#define GRID_CELL_SIZE 100

void draw()
{

}

void step()
{
    
}

int main()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	}

	SDL_Window* win = SDL_CreateWindow("Snake", // creates a window 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									GRID_CELL_SIZE * GRID_RES_X, 
                                    GRID_CELL_SIZE * GRID_RES_Y, 0);
    int should_close = 0;

    //main loop
    while (should_close == 0)
    {
        
    }
}