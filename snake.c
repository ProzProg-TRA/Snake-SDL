#include <SDL2/SDL.h>

#define GRID_RES_X 180
#define GRID_RES_Y 80    
#define GRID_CELL_SIZE 10
#define NUM_TICKS_PER_FRAME 400

void restart();

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* wall_tex;
SDL_Texture* item_tex;
SDL_Texture* snake_tex;
SDL_Texture* game_over_tex;
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

enum cell_state grid[GRID_RES_X * GRID_RES_Y];

enum directions snake_direction;
snake_segment* snake_head;
int score;
int item_eaten;
int should_close;
int pause;
int game_over;

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

    if (game_over)
    {
        SDL_RenderCopy(renderer, game_over_tex, NULL, NULL);
        SDL_RenderPresent(renderer);
        return;
    }
    

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
    enum directions new_direction = snake_direction;

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
                if (snake_direction != down)
                {
                    new_direction = up;
                }
                break;
            case SDL_SCANCODE_DOWN:
                if (snake_direction != up)
                {
                    new_direction = down;
                }
                break;
            case SDL_SCANCODE_LEFT:
                if (snake_direction != right)
                {
                    new_direction = left;
                }
                break;
            case SDL_SCANCODE_RIGHT:
                if (snake_direction != left)
                {
                    new_direction = right;
                }
                break;
            case SDL_SCANCODE_RETURN:
                if (game_over)
                {
                    restart();
                    break;
                }
                pause = !pause;
                break;
            case SDL_SCANCODE_ESCAPE:
                should_close = 1;
                break;
            default:
                break;
            }
        default:
            break;
        }
    }

    snake_direction = new_direction;

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
        game_over = 1;
        break;
    case empty:
        // do nothing
        // fall through
    default:
        break;
    }

    if (game_over)
    {
        return;
    }

    add_new_head(new_head_x, new_head_y);
    if (item_eaten == 1)
    {
        place_new_item();
        return;
    }

    remove_last_segment();
}

void init_grid()
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
    snake_direction = left;
    
    add_new_head(8, 4);
    add_new_head(7, 4);
    
}

void destroy_snake(snake_segment* seg)
{
    if (seg->next == NULL)
    {
        free(seg);
        return;
    }
    destroy_snake(seg->next);
}

void restart()
{
    // delete current snake
    destroy_snake(snake_head);

    // clear the grid
    for (int y = 0; y < GRID_RES_Y; y++)
    {
        for (int x = 0; x < GRID_RES_X; x++)
        {
            *get_cell_ptr_at(x, y) = empty;
        }
        
    }

    // initialize the grid
    init_grid();

    game_over = 0;
}

int main(int argc, char *argv[])
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

    surface = SDL_LoadBMP("res/snake.bmp");
    snake_tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = SDL_LoadBMP("res/game_over.bmp");
    game_over_tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // initialize
    pause = 1;
    should_close = 0;
    game_over = 0;
    
    unsigned int tick;
    int remaining;

    init_grid();

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