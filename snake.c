#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>

#define GRID_RES_X 16
#define GRID_RES_Y 9    
#define GRID_CELL_SIZE 50

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* wall_tex;
SDL_Surface* wall_surface;
SDL_Event event;

enum directions{
    up      = 0,
    down    = 1,
    left    = 2,
    right   = 3
};

/*
 * Format of the grid cells:
 * -------------------------------
 * >0 = Snake, TTL of the segment
 *  0 = Empty
 * -1 = Item
 * -2 = Wall
 */

int grid[GRID_RES_X * GRID_RES_Y];

enum directions snake_direction;
int snake_head_x;
int snake_head_y;

int should_close;
int pause;

void draw()
{
    SDL_RenderClear(renderer);
    SDL_Rect rect;
    rect.w = rect.h = GRID_CELL_SIZE;

    for (int y = 0; y < GRID_RES_Y; y++)
    {
        for (int x = 0; x < GRID_RES_X; x++)
        {
            rect.x = x * GRID_CELL_SIZE;
            rect.y = y * GRID_CELL_SIZE;

            switch (grid[y*GRID_RES_X+x])
            {
            case -2:
                SDL_RenderCopy(renderer, wall_tex, NULL, &rect);
                break;
            case -1:
                // render item
                break;
            case 0:
                // render background
    grid[5*GRID_RES_X+3] = -1;
                break;
            default:
                break;
                // render snake
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

void init_board()
{
    // create walls
    for (int y = 0; y < GRID_RES_Y; y++)
    {
        for (int x = 0; x < GRID_RES_X; x++)
        {
            if (x == 0 || x == GRID_RES_X - 1 || y == 0 || y == GRID_RES_Y - 1)
            {
                grid[y * GRID_RES_X + x] = -2;
            }
        }
    }
    
    // create item
    grid[5*GRID_RES_X+3] = -1;
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

    wall_surface = SDL_LoadBMP("res/red.bmp");
    wall_tex = SDL_CreateTextureFromSurface(renderer, wall_surface);
    SDL_FreeSurface(wall_surface);

    pause = 0;
    should_close = 0;

    init_board();

    //main loop
    while (should_close == 0)
    {
        step();
        draw();
    }
}