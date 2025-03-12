#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int screenWidth = 640;
int screenHeight = 480;

float playerX = 3, playerY = 2.5;
float dirX = 0, dirY = -1;
float planeX = .85, planeY = 0;

int map[8][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 2, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 2, 1},
    {1, 2, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

int wallColors[][3] = {
    {128, 0, 128},
    {223, 12, 68},
};

void drawline(float x, float y1, float y2, int color0, int color1, int color2) {
    SDL_SetRenderDrawColor(renderer, color0, color1, color2, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, x, y1, x, y2);
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

            if (map[mapY][mapX]) {
                hit = map[mapY][mapX];
            }
        }
        
        if (side == 0) {
            perpWallDist = sideDistX - deltaDistX;
        } else {
            perpWallDist = sideDistY - deltaDistY;
        }

        float lineHeight = screenHeight / perpWallDist;

        float drawStart = (-lineHeight / 2 + screenHeight / 2);
        float drawEnd = (lineHeight / 2 + screenHeight / 2);

        drawStart = SDL_max(drawStart, 0);
        drawEnd = SDL_min(drawEnd, screenHeight - 1);

        int color[] = {wallColors[hit - 1][0], wallColors[hit - 1][1], wallColors[hit - 1][2]};

        if (side) {
            color[0] = color[0] * .8;
            color[1] = color[1] * .8;
            color[2] = color[2] * .8;
        }
        
        drawline(x, drawStart, drawEnd, color[0], color[1], color[2]);
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        raycast();

        SDL_RenderPresent(renderer);
    } 
}

int main() {
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