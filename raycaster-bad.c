#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

#define mapWidth 8
#define mapHeight 8
#define screenWidth 640
#define screenHeight 480

int map[mapWidth][mapHeight] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 2, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 2, 1},
    {1, 2, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

double playerX = 4, playerY = 4;
double dirX = 0, dirY = -1;
double planeX = .85, planeY = 0;

double time = 0;
double oldTime = 0;

int wallColors[][3] = {
    {128, 0, 128},
    {223, 12, 68},
};

// w, a, s, d, arrow left, arrow right
int inputs[] = {0, 0, 0, 0, 0, 0};

void drawline(double x, double y1, double y2, int color[]) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, x, y1, x, y2);
};

void raycast() {
    for (int x = 0; x < screenWidth; x++) {
        double cameraX = 2 * x / screenWidth - 1;

        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = SDL_floor(playerX);
        int mapY = SDL_floor(playerY);

        double sideDistX, sideDistY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : SDL_abs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : SDL_abs(1 / rayDirY);

        double perpWallDist;

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

        int lineHeight = (int)(screenHeight / perpWallDist);

        int drawStart = -lineHeight / 2 + screenHeight / 2;
        int drawEnd = lineHeight / 2 + screenHeight / 2;

        if (drawStart < 0) { drawStart = 0; }
        if (drawEnd >= screenHeight) { drawEnd = screenHeight - 1; }

        // drawStart = SDL_max(drawStart, 0);
        // drawEnd = SDL_min(drawEnd, screenHeight - 1);

        int color[] = {wallColors[hit - 1][0], wallColors[hit - 1][1], wallColors[hit - 1][2]};

        if (side) {
            color[0] = color[0] * .8;
            color[1] = color[1] * .8;
            color[2] = color[2] * .8;
        }
        
        drawline(x, drawStart, drawEnd, color);
    }
}

void movePlayer() {
    oldTime = time;
    time = SDL_GetTicks();
    double deltaTime = (time - oldTime) / 1000.0;

    double moveSpeed = deltaTime * 5.0;
    double rotateSpeed = deltaTime * 2.5;

    // i should change this to something that gets calculated depending on moveSpeed instead of a hardcoded value that only works with a moveSpeed of 5
    double diagonalPenalty = 0.708;

    if (inputs[0] && inputs[1] && !inputs[3] ||
        inputs[0] && inputs[3] && !inputs[1] ||
        inputs[2] && inputs[1] && !inputs[3] ||
        inputs[2] && inputs[3] && !inputs[1]
    ) {
        moveSpeed = moveSpeed * diagonalPenalty;
    }

    // there has to be a better way to do this
    if (inputs[0]) {
        if (map[(int)SDL_floor(playerY)][(int)SDL_floor(playerX + dirX * moveSpeed)] == 0) { playerX += dirX * moveSpeed; }
        if (map[(int)SDL_floor(playerY + dirY * moveSpeed)][(int)SDL_floor(playerX)] == 0) { playerY += dirY * moveSpeed; }      
    }

    if (inputs[1]) {
        if (map[(int)SDL_floor(playerY)][(int)SDL_floor(playerX - dirX * moveSpeed)] == 0) { playerX -= dirX * moveSpeed; }
        if (map[(int)SDL_floor(playerY - dirY * moveSpeed)][(int)SDL_floor(playerX)] == 0) { playerY -= dirY * moveSpeed; }      
    }

    if (inputs[2]) {
        if (map[(int)SDL_floor(playerY)][(int)SDL_floor(playerX + dirX * moveSpeed)] == 0) { playerX += dirX * moveSpeed; }
        if (map[(int)SDL_floor(playerY - dirY * moveSpeed)][(int)SDL_floor(playerX)] == 0) { playerY -= dirY * moveSpeed; }      
    }

    if (inputs[3]) {
        if (map[(int)SDL_floor(playerY)][(int)SDL_floor(playerX - dirX * moveSpeed)] == 0) { playerX -= dirX * moveSpeed; }
        if (map[(int)SDL_floor(playerY + dirY * moveSpeed)][(int)SDL_floor(playerX)] == 0) { playerY += dirY * moveSpeed; }      
    }

    if (inputs[4]) {
        double oldDirX = dirX;
        dirX = dirX * SDL_cos(-rotateSpeed) - dirY * SDL_sin(-rotateSpeed);
        dirY = oldDirX * SDL_sin(-rotateSpeed) + dirY * SDL_cos(-rotateSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * SDL_cos(-rotateSpeed) - planeY * SDL_sin(-rotateSpeed);
        planeY = oldPlaneX * SDL_sin(-rotateSpeed) + planeY * SDL_cos(-rotateSpeed);
    }

    if (inputs[5]) {
        double oldDirX = dirX;
        dirX = dirX * SDL_cos(rotateSpeed) - dirY * SDL_sin(rotateSpeed);
        dirY = oldDirX * SDL_sin(rotateSpeed) + dirY * SDL_cos(rotateSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * SDL_cos(rotateSpeed) - planeY * SDL_sin(rotateSpeed);
        planeY = oldPlaneX * SDL_sin(rotateSpeed) + planeY * SDL_cos(rotateSpeed);
    }
}

void loop() {
    SDL_Event e;
    SDL_zero(e);

    bool quit = false;

    while (!quit) {
        
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT ||
                e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE
            ) {
                quit = true;
            }

            if (e.type == SDL_EVENT_KEY_DOWN) {
                switch (e.key.scancode) {
                    case SDL_SCANCODE_W:
                        inputs[0] = 1;
                        break;
                    case SDL_SCANCODE_A:
                        inputs[1] = 1;
                        break;
                    case SDL_SCANCODE_S:
                        inputs[2] = 1;
                        break;
                    case SDL_SCANCODE_D:
                        inputs[3] = 1;
                        break;
                    case SDL_SCANCODE_LEFT:
                        inputs[4] = 1;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        inputs[5] = 1;
                        break;
                    
                    default:
                        break;
                }
            }

            if (e.type == SDL_EVENT_KEY_UP) {
                switch (e.key.scancode) {
                    case SDL_SCANCODE_W:
                        inputs[0] = 0;
                        break;
                    case SDL_SCANCODE_A:
                        inputs[1] = 0;
                        break;
                    case SDL_SCANCODE_S:
                        inputs[2] = 0;
                        break;
                    case SDL_SCANCODE_D:
                        inputs[3] = 0;
                        break;
                    case SDL_SCANCODE_LEFT:
                        inputs[4] = 0;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        inputs[5] = 0;
                        break;
                    
                    default:
                        break;
                }
            }
            
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        raycast();
        movePlayer(inputs);

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