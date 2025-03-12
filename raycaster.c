#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int screenWidth = 640;
int screenHeight = 480;

void raycast() {
    for (int i = 0; i < screenWidth; i++) {
        /* code */
    }
    
}

void loop() {
    SDL_Event e;
    SDL_zero(e);

    bool quit = false;

    while (!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(renderer);

        raycast();

        SDL_RenderPresent(renderer);
    } 
}

int main() {
    float playerX = 2.5, playerY = 13;
    float dirX = 0, dirY = -1;
    float planeX = .85, planeY = 0;

    int mapSize[] = {8, 8};
    int map[] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 2, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 2, 0, 1,
        1, 0, 0, 0, 0, 2, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 2, 2, 2, 1,
        1, 2, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };

    int wallColors[] = {
        128, 0, 128,
        223, 12, 68,
    };

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("initialization failed: %s\n", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer("epic window :fire: :flag_tf:", screenWidth, screenHeight, 0, &window, &renderer)) {
        SDL_Log("create window/renderer failed: %s", SDL_GetError());
    }

    loop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}