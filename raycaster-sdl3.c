#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

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
    {1, 1, 1, 1, 1, 1, 1, 1}};

int wallColors[][3] = {
    {128, 0, 128},
    {223, 12, 68},
};

// w, a, s, d, arrow left, arrow right
int inputs[] = {0, 0, 0, 0, 0, 0};

int main()
{
    double playerX = 2.5, playerY = 2.5;
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;

    double time = 0;
    double oldTime = 0;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("initialization failed: %s\n", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer("epic window :fire: :flag_tf:", screenWidth, screenHeight, 0, &window, &renderer))
    {
        SDL_Log("create window/renderer failed: %s", SDL_GetError());
    }

    // grab mouse
    // SDL_SetWindowMouseGrab(window, true);

    SDL_Event e;
    SDL_zero(e);

    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT ||
                e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)
            {
                quit = true;
            }

            if (e.type == SDL_EVENT_KEY_DOWN)
            {
                switch (e.key.scancode)
                {
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

            if (e.type == SDL_EVENT_KEY_UP)
            {
                switch (e.key.scancode)
                {
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

            if (e.type == SDL_EVENT_MOUSE_MOTION)
            {
                SDL_Log("%lf", e.motion.x);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int x = 0; x < screenWidth; x++)
        {
            double cameraX = 2 * x / (double)screenWidth - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)playerX;
            int mapY = (int)playerY;

            double sideDistX;
            double sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : SDL_fabs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : SDL_fabs(1 / rayDirY);
            double perpWallDist;

            int stepX;
            int stepY;

            int hit = 0;
            int side;

            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (playerX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
            }

            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (playerY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
            }

            while (hit == 0)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                if (map[mapX][mapY] > 0)
                {
                    hit = map[mapX][mapY];
                }
            }

            if (side == 0)
            {
                perpWallDist = (sideDistX - deltaDistX);
            }
            else
            {
                perpWallDist = (sideDistY - deltaDistY);
            }

            int lineHeight = (int)(screenHeight / perpWallDist);

            int drawStart = -lineHeight / 2 + screenHeight / 2;
            if (drawStart < 0)
            {
                drawStart = 0;
            }
            int drawEnd = lineHeight / 2 + screenHeight / 2;
            if (drawEnd >= screenHeight)
            {
                drawEnd = screenHeight - 1;
            }

            int color[] = {wallColors[hit - 1][0], wallColors[hit - 1][1], wallColors[hit - 1][2]};

            if (side)
            {
                color[0] = color[0] * .8;
                color[1] = color[1] * .8;
                color[2] = color[2] * .8;
            }

            SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], SDL_ALPHA_OPAQUE);
            SDL_RenderLine(renderer, x, drawStart, x, drawEnd);
        }

        SDL_RenderPresent(renderer);

        oldTime = time;
        time = SDL_GetTicks();
        double deltaTime = (time - oldTime) / 1000;

        double moveSpeed = deltaTime * 5.0;
        double rotateSpeed = deltaTime * 3.0;

        double diagonalPenalty = 0.708;

        if (inputs[0] && inputs[1] && !inputs[2] && !inputs[3] ||
            inputs[0] && !inputs[1] && !inputs[2] && inputs[3] ||
            !inputs[0] && inputs[1] && inputs[2] && !inputs[3] ||
            !inputs[0] && !inputs[1] && inputs[2] && inputs[3])
        {
            moveSpeed = moveSpeed * diagonalPenalty;
        }

        // forward
        if (inputs[0])
        {
            if (map[(int)SDL_floor(playerX + dirX * moveSpeed)][(int)SDL_floor(playerY)] == 0)
            {
                playerX += dirX * moveSpeed;
            }
            if (map[(int)SDL_floor(playerX)][(int)SDL_floor(playerY + dirY * moveSpeed)] == 0)
            {
                playerY += dirY * moveSpeed;
            }
        }

        // left
        if (inputs[1])
        {
            if (map[(int)SDL_floor(playerX - dirY * moveSpeed)][(int)SDL_floor(playerY)] == 0)
            {
                playerX -= dirY * moveSpeed;
            }
            if (map[(int)SDL_floor(playerX)][(int)SDL_floor(playerY + dirX * moveSpeed)] == 0)
            {
                playerY += dirX * moveSpeed;
            }
        }

        // backward
        if (inputs[2])
        {
            if (map[(int)SDL_floor(playerX - dirX * moveSpeed)][(int)SDL_floor(playerY)] == 0)
            {
                playerX -= dirX * moveSpeed;
            }
            if (map[(int)SDL_floor(playerX)][(int)SDL_floor(playerY - dirY * moveSpeed)] == 0)
            {
                playerY -= dirY * moveSpeed;
            }
        }

        // right
        if (inputs[3])
        {
            if (map[(int)SDL_floor(playerX + dirY * moveSpeed)][(int)SDL_floor(playerY)] == 0)
            {
                playerX += dirY * moveSpeed;
            }
            if (map[(int)SDL_floor(playerX)][(int)SDL_floor(playerY - dirX * moveSpeed)] == 0)
            {
                playerY -= dirX * moveSpeed;
            }
        }

        // rotate left
        if (inputs[4])
        {
            double oldDirX = dirX;
            dirX = dirX * SDL_cos(rotateSpeed) - dirY * SDL_sin(rotateSpeed);
            dirY = oldDirX * SDL_sin(rotateSpeed) + dirY * SDL_cos(rotateSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * SDL_cos(rotateSpeed) - planeY * SDL_sin(rotateSpeed);
            planeY = oldPlaneX * SDL_sin(rotateSpeed) + planeY * SDL_cos(rotateSpeed);
        }

        // rotate right
        if (inputs[5])
        {
            double oldDirX = dirX;
            dirX = dirX * SDL_cos(-rotateSpeed) - dirY * SDL_sin(-rotateSpeed);
            dirY = oldDirX * SDL_sin(-rotateSpeed) + dirY * SDL_cos(-rotateSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * SDL_cos(-rotateSpeed) - planeY * SDL_sin(-rotateSpeed);
            planeY = oldPlaneX * SDL_sin(-rotateSpeed) + planeY * SDL_cos(-rotateSpeed);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}