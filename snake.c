#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define GRID_SIZE 20
#define GRID_DIM 800

enum
{
    SNAKE_U,
    SNAKE_D,
    SNAKE_L,
    SNAKE_R,
};

typedef struct
{
    int x;
    int y;
} apple;
apple Apple;

struct snake
{
    int x;
    int y;
    int dir;
    uint8_t r_color;
    uint8_t g_color;
    uint8_t b_color;
    uint8_t a_color;
    struct snake *next;
};

typedef struct snake Snake;
Snake *head;
Snake *tail;

bool quit = false;

void init_snake()
{
    Snake *new = malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->r_color = rand() * 255;
    new->g_color = rand() * 255;
    new->b_color = rand() * 255;
    new->a_color = rand() * 255;
    new->dir = SNAKE_U;
    new->next = NULL;
    head = new;
    tail = new;
    return;
}
void increase_snake()
{
    Snake *new = malloc(sizeof(Snake));
    new->x = tail->x;
    new->y = tail->y;
    new->r_color = rand() * 255;
    new->g_color = rand() * 255;
    new->b_color = rand() * 255;
    new->a_color = rand() * 255;
    new->next = NULL;
    tail->next = new;
    tail = new;
}
void render_snake(SDL_Renderer *rendrer, int x, int y)
{
    int seg_size = GRID_DIM / GRID_SIZE;
    SDL_Rect seg;

    seg.w = seg_size;
    seg.h = seg_size;

    Snake *track = head;

    while (track != NULL)
    {
        seg.x = x + track->x * seg_size;
        seg.y = y + track->y * seg_size;
        SDL_SetRenderDrawColor(rendrer, track->r_color, track->g_color, track->b_color, track->a_color);
        SDL_RenderFillRect(rendrer, &seg);
        SDL_SetRenderDrawColor(rendrer, 255, 255, 255, 255);
        SDL_RenderDrawRect(rendrer, &seg);
        track = track->next;
    }

    return;
}
void render_grid(SDL_Renderer *rendrer, int x, int y)
{
    SDL_SetRenderDrawColor(rendrer, 0x55, 0x55, 0x55, 255);
    int cell_size = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cell.x = x + (i * cell_size);
            cell.y = y + (j * cell_size);
            SDL_RenderDrawRect(rendrer, &cell);
        }
    }

    return;
}

void move_snake()
{
    int prev_x = head->x;
    int prev_y = head->y;
    switch (head->dir)
    {
    case SNAKE_U:
        head->y--;
        break;
    case SNAKE_D:
        head->y++;
        break;
    case SNAKE_L:
        head->x--;
        break;
    case SNAKE_R:
        head->x++;
        break;
    }
    Snake *track = head;
    if (track->next != NULL)
    {
        track = track->next;
    }
    while (track != NULL)
    {
        int save_x = track->x;
        int save_y = track->y;

        track->x = prev_x;
        track->y = prev_y;

        track = track->next;
        prev_x = save_x;
        prev_y = save_y;
    }
}

void gen_apple()
{
    Apple.x = rand() % GRID_SIZE;
    Apple.y = rand() % GRID_SIZE;
}

void render_apple(SDL_Renderer *rendrer, int x, int y)
{
    SDL_SetRenderDrawColor(rendrer, 0x00, 0x55, 0x55, 255);
    int apple_size = GRID_DIM / GRID_SIZE;
    SDL_Rect app;
    app.w = apple_size;
    app.h = apple_size;
    app.x = x + Apple.x * apple_size;
    app.y = y + Apple.y * apple_size;
    SDL_RenderFillRect(rendrer, &app);
}

void detect_apple()
{
    if (head->x == Apple.x && head->y == Apple.y)
    {
        gen_apple();
        increase_snake();
    }
    return;
}

void detect_boundaries()
{
    if (head->x > GRID_DIM || head->x < 0 || head->y > GRID_DIM || head->y < 0)
    {
        quit = true;
    }
    return;
}
void detect_snake_collision()
{
    if (head->next != NULL)
    {
        Snake *track = head->next;
        while (track->next != NULL)
        {
            if (head->x == track->x && head->y == track->y)
            {
                quit = true;
            }
            track = track->next;
        }
    }
    return;
}

void renderText(SDL_Renderer *renderer, const char *text, int x, int y, SDL_Color color, TTF_Font *font)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface == NULL)
    {
        fprintf(stderr, "TTF_RenderText_Solid Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL)
    {
        fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};

    // Render the text on the renderer
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Clean up resources
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

int WinMain(int argc, char *argv[])
{
    if (argc && argv)
    {
    }
    srand(time(0));
    init_snake();
    increase_snake();
    gen_apple();
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;

    window = SDL_CreateWindow(
        "Snake",
        WINDOW_X,
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!window)
    {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1; // Exit with an error code
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1; // Exit with an error code
    }

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("fonts/Ubuntu-Regular.ttf", 24);
    if (font == NULL)
    {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Start Event management to quit or termnate the windows
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_UP:
                    head->dir = SNAKE_U;
                    break;
                case SDLK_DOWN:
                    head->dir = SNAKE_D;
                    break;
                case SDLK_LEFT:
                    head->dir = SNAKE_L;
                    break;
                case SDLK_RIGHT:
                    head->dir = SNAKE_R;
                    break;
                }
                break;
            }
        }

        SDL_RenderClear(renderer);

        // render loop start
        SDL_Color textColor = {255, 255, 255, 255};
        char str[20];
        snprintf(str, sizeof(str), "x : %d y : %d", head->x, head->y);
        renderText(renderer, str, (WINDOW_WIDTH / 2) - (GRID_DIM / 2), (WINDOW_HEIGHT / 2) - (GRID_DIM / 2) - 50, textColor, font);
        move_snake();
        detect_apple();
        detect_boundaries(&quit);
        detect_snake_collision(&quit);

        // render grid in centred mode

        render_grid(renderer, ((WINDOW_WIDTH / 2) - (GRID_DIM / 2)), ((WINDOW_HEIGHT / 2) - (GRID_DIM / 2)));
        render_snake(renderer, ((WINDOW_WIDTH / 2) - (GRID_DIM / 2)), ((WINDOW_HEIGHT / 2) - (GRID_DIM / 2)));
        render_apple(renderer, ((WINDOW_WIDTH / 2) - (GRID_DIM / 2)), ((WINDOW_HEIGHT / 2) - (GRID_DIM / 2)));

        // render loop end

        SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 255);
        SDL_RenderPresent(renderer);
        SDL_Delay(200);
    }
    // End  Event management to quit or termnate the windows
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
