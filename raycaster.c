#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int screenWidth = 640;
int screenHeight = 480;

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

void raycast() {
    for (int x = 0; x < screenWidth; x++) {
        float cameraX = 2 * x / screenWidth - 1;

        float rayDirX = dirX + planeX * cameraX;
        float rayDirY = dirY + planeY * cameraX;

        int mapX = SDL_floor(playerX);
        int mapY = SDL_floor(playerY);

        float sideDistX, sideDistY;

        float deltaDistX = SDL_abs(1 / rayDirX);
        float deltaDistY = SDL_abs(1 / rayDirY);

        float perpWallDist;

        int stepX, stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (playerX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1 - playerX) * deltaDistX;
        }

        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (playerY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1 - playerY) * deltaDistY;
        };

        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (map[(mapY * mapSize[1]) + mapX] != 0) {
                hit = 1;
            }
        }
        
        if (side == 0) {
            perpWallDist = sideDistX - deltaDistX;
        } else {
            perpWallDist = sideDistY - deltaDistY;
        }

        float lineHeight = screenHeight / perpWallDist;

        float drawStart = -lineHeight / 2 + screenHeight / 2;
        float drawEnd = lineHeight / 2 + screenHeight / 2;

        drawStart = SDL_max(drawStart, 0);
        drawEnd = SDL_min(drawEnd, screenHeight - 1);
        
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