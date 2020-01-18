#include <SDL2/SDL.h>

#define GRID_RES_X 16
#define GRID_RES_Y 9    
#define GRID_CELL_SIZE 50
#define NUM_TICKS_PER_FRAME 200

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* wall_tex;
SDL_Texture* item_tex;
SDL_Texture* snake_tex;
SDL_Surface* surface;
SDL_Event event;

enum directions{
    up,
    down,
    left,
    right
};

enum cell_state{
    empty,
    snake,
    wall,
    item
};

struct _snake_segment{
    int x;
    int y;
    struct _snake_segment* next;
};

typedef struct _snake_segment snake_segment;

/*
 * Format of the grid cells:
 * -------------------------------
 *  1 = Snake
 *  0 = Empty
 * -1 = Item
 * -2 = Wall
 */

enum cell_state grid[GRID_RES_X * GRID_RES_Y];

enum directions snake_direction;
snake_segment* snake_head;
int snake_length;
int item_eaten;
int should_close;
int pause;

enum cell_state* get_cell_ptr_at(int x, int y)
{
    return &(grid[y*GRID_RES_X+x]);
}

void add_new_head(int x, int y)
{
    snake_segment* new_head = malloc(sizeof(snake_segment));

    new_head->next = snake_head;
    new_head->x = x;
    new_head->y = y;    
    snake_head = new_head;
    *get_cell_ptr_at(new_head->x, new_head->y) = snake;
}

void remove_last_segment()
{
    snake_segment* cur = snake_head;
    while (cur->next->next != NULL)
    {
        cur = cur->next;
    }
    *get_cell_ptr_at(cur->next->x, cur->next->y) = 0;
    free(cur->next);
    cur->next = NULL;
}

void place_new_item()
{
    // find all empty cells
    enum cell_state* empty_cells[(GRID_RES_X - 2) * (GRID_RES_Y - 2)];
    enum cell_state* cur;
    int i = 0;

    for (int y = 1; y < GRID_RES_Y - 1; y++)
    {
        for (int x = 1; x < GRID_RES_X - 1; x++)
        {
            cur = get_cell_ptr_at(x, y);
            if (*cur == empty)
            {
                empty_cells[i] = cur;
                i++;
            }
        }
    }
    
    // choose one of the empty cells
    *empty_cells[rand() % i] = item;
}

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

            switch (*get_cell_ptr_at(x, y))
            {
            case wall:
                SDL_RenderCopy(renderer, wall_tex, NULL, &rect);
                break;
            case item:
                SDL_RenderCopy(renderer, item_tex, NULL, &rect);
                break;
            case empty:
                // render background
                break;
            case snake:
                SDL_RenderCopy(renderer, snake_tex, NULL, &rect);
                break;
            default:
                break;
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void step()
{
    while(SDL_PollEvent(&event)){
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
            case SDL_SCANCODE_ESCAPE:
                pause = !pause;
                break;
            default:
                break;
            }
        default:
            break;
        }
    }

    if (pause)
    {
        return;
    }

    int new_head_x = snake_head->x;
    int new_head_y = snake_head->y;

    switch (snake_direction)
    {
    case up:
        new_head_y -= 1;
        break;
    case down:
        new_head_y += 1;
        break;
    case left:
        new_head_x -= 1;
        break;
    case right:
        new_head_x += 1;
        break;
    default:
        break;
    }

    // check for collisions
    item_eaten = 0;
    switch (*get_cell_ptr_at(new_head_x, new_head_y))
    {
    case item:
        item_eaten = 1;
        break;
    case wall:
        // fall through
    case snake:
        // TODO: game over
        break;
    case empty:
        // do nothing
        // fall through
    default:
        break;
    }

    add_new_head(new_head_x, new_head_y);
    if (item_eaten == 1)
    {
        place_new_item();
        return;
    }

    remove_last_segment();
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
                *get_cell_ptr_at(x, y) = wall;
            }
        }
    }
    
    // create item
    *get_cell_ptr_at(3, 5) = item;

    // create snake
    snake_segment* seg1 = malloc(sizeof(snake_segment));
    seg1->next = NULL;
    seg1->x = 9;
    seg1->y = 4;
    *get_cell_ptr_at(9, 4) = snake;
    
    snake_head = seg1;
    
    add_new_head(8, 4);
    add_new_head(7, 4);
    
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

    // load the textures
    surface = SDL_LoadBMP("res/wall.bmp");
    wall_tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = SDL_LoadBMP("res/item.bmp");
    item_tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = SDL_LoadBMP("res/red.bmp");
    snake_tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);


    pause = 1;
    should_close = 0;
    snake_direction = left;
    
    unsigned int tick;
    int remaining;

    init_board();

    //main loop
    while (should_close == 0)
    {
        tick = SDL_GetTicks();
        step();
        draw();
        remaining = NUM_TICKS_PER_FRAME - (SDL_GetTicks() - tick);
        if (remaining > 0)
        {
            SDL_Delay(remaining);
        }
    }
}