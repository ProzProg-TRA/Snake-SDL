#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>

#define GRID_RES_X 16
#define GRID_RES_Y 9    
#define GRID_CELL_SIZE 50

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* bitmapTex;
SDL_Surface* bitmapSurface;
SDL_Event event;

enum directions{
    up      = 0,
    down    = 1,
    left    = 2,
    right   = 3
};

enum directions snake_direction;
int should_close;
int pause;

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
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        should_close = 1;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_UP:
            snake_direction = up;
            break;
        case SDL_SCANCODE_DOWN:
            snake_direction = down;
            break;
        case SDL_SCANCODE_LEFT:
            snake_direction = left;
            break;
        case SDL_SCANCODE_RIGHT:
            snake_direction = right;
            break;
        default:
            break;
        }
    default:
        break;
    }
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

    pause = 0;
    should_close = 0;

    //main loop
    while (should_close == 0)
    {
        step();
        draw();
    }
}