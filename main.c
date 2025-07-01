#include "raylib.h"
#include <math.h>

#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define RAY_DENSITY 1

#define MAP_HEIGHT 9
#define MAP_WIDTH 9

#define PLAYER_SIZE .04

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 9, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int wallColors[][3] = {
    {128, 0, 128},
    {223, 12, 68},
};

struct InputStruct
{
    int w;
    int a;
    int s;
    int d;
    int left;
    int right;
};

int main(void)
{
    struct InputStruct inputs = {0, 0, 0, 0, 0, 0};

    int renderWidth = SCREEN_WIDTH, renderHeight = SCREEN_HEIGHT;
    int windowHeight = SCREEN_HEIGHT;

    double playerX = 4.5, playerY = 3.5;
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;

    double rainbow = 0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(renderWidth, renderHeight, "raycaster");

    while (!WindowShouldClose())
    {
        // if window was resized previous frame update values used for casting rays
        if (IsWindowResized())
        {
            renderWidth = GetScreenWidth();
            renderHeight = renderWidth * 3 / 4;
            windowHeight = GetScreenHeight();
        }

        inputs.w = IsKeyDown(KEY_W) ? 1 : 0;
        inputs.a = IsKeyDown(KEY_A) ? 1 : 0;
        inputs.s = IsKeyDown(KEY_S) ? 1 : 0;
        inputs.d = IsKeyDown(KEY_D) ? 1 : 0;
        inputs.left = IsKeyDown(KEY_LEFT) ? 1 : 0;
        inputs.right = IsKeyDown(KEY_RIGHT) ? 1 : 0;

        BeginDrawing();
        ClearBackground(BLANK);

        for (int x = 0; x <= renderWidth / RAY_DENSITY; x++)
        {
            double cameraX = 2 * x / (double)(renderWidth / RAY_DENSITY) - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)playerX;
            int mapY = (int)playerY;

            double sideDistX;
            double sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
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

            int lineHeight = (int)(renderHeight / perpWallDist);

            int drawStart = -lineHeight / 2 + renderHeight / 2;
            if (drawStart < 0)
            {
                drawStart = 0;
            }

            int drawEnd = lineHeight / 2 + renderHeight / 2;
            if (drawEnd >= windowHeight)
            {
                drawEnd = windowHeight - 1;
            }

            if (hit == 9)
            {
                DrawLine(x, drawStart, x, drawEnd, ColorFromHSV(rainbow, 1, 1));
            }
            else
            {
                int color[] = {wallColors[hit - 1][0], wallColors[hit - 1][1], wallColors[hit - 1][2]};

                if (side)
                {
                    color[0] = color[0] * .8;
                    color[1] = color[1] * .8;
                    color[2] = color[2] * .8;
                }

                // if (RAY_DENSITY == 1)
                // {
                DrawLine(x, drawStart, x, drawEnd, (Color){color[0], color[1], color[2], 255});
                printf("%d %d\n", drawStart, drawEnd);
                // }
                // else
                // {
                //     for (int i = 0; i < RAY_DENSITY; i++)
                //     {
                //         DrawLine(x * RAY_DENSITY + i, drawStart, x * RAY_DENSITY + i, drawEnd, (Color){color[0], color[1], color[2], 255});
                //     }
                // }
            }
        }

        DrawFPS(0, 0);

        EndDrawing();

        double deltaTime = GetFrameTime();
        // printf("%f %f %f %f %f %f\n", playerX,playerY,dirX,dirY,planeX,planeY);

        rainbow += deltaTime * 64;

        double moveSpeed = deltaTime * 5.0;
        double rotateSpeed = deltaTime * 3.0;

        double diagonalPenalty = 0.708;

        // apply movement penalty if 2 
        if (inputs.w && inputs.a && !inputs.s && !inputs.d ||
            inputs.w && !inputs.a && !inputs.s && inputs.d ||
            !inputs.w && inputs.a && inputs.s && !inputs.d ||
            !inputs.w && !inputs.a && inputs.s && inputs.d)
        {
            moveSpeed = moveSpeed * diagonalPenalty;
        }

        // forward
        if (inputs.w)
        {
            if (map[(int)floor((playerX + PLAYER_SIZE) + dirX * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX + PLAYER_SIZE) + dirX * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) + dirX * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) + dirX * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0)
            {
                playerX += dirX * moveSpeed;
            }
            if (map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) + dirY * moveSpeed)] == 0 &&
                map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) + dirY * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) + dirY * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) + dirY * moveSpeed)] == 0)
            {
                playerY += dirY * moveSpeed;
            }
        }

        // left
        if (inputs.a)
        {
            if (map[(int)floor((playerX + PLAYER_SIZE) - dirY * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX + PLAYER_SIZE) - dirY * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) - dirY * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) - dirY * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0)
            {
                playerX -= dirY * moveSpeed;
            }
            if (map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) + dirX * moveSpeed)] == 0 &&
                map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) + dirX * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) + dirX * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) + dirX * moveSpeed)] == 0)
            {
                playerY += dirX * moveSpeed;
            }
        }

        // backward
        if (inputs.s)
        {
            if (map[(int)floor((playerX + PLAYER_SIZE) - dirX * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX + PLAYER_SIZE) - dirX * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) - dirX * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) - dirX * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0)
            {
                playerX -= dirX * moveSpeed;
            }
            if (map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) - dirY * moveSpeed)] == 0 &&
                map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) - dirY * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) - dirY * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) - dirY * moveSpeed)] == 0)
            {
                playerY -= dirY * moveSpeed;
            }
        }

        // right
        if (inputs.d)
        {
            if (map[(int)floor((playerX + PLAYER_SIZE) + dirY * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX + PLAYER_SIZE) + dirY * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) + dirY * moveSpeed)][(int)floor(playerY + PLAYER_SIZE)] == 0 &&
                map[(int)floor((playerX - PLAYER_SIZE) + dirY * moveSpeed)][(int)floor(playerY - PLAYER_SIZE)] == 0)
            {
                playerX += dirY * moveSpeed;
            }
            if (map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) - dirX * moveSpeed)] == 0 &&
                map[(int)floor(playerX + PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) - dirX * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY + PLAYER_SIZE) - dirX * moveSpeed)] == 0 &&
                map[(int)floor(playerX - PLAYER_SIZE)][(int)floor((playerY - PLAYER_SIZE) - dirX * moveSpeed)] == 0)
            {
                playerY -= dirX * moveSpeed;
            }
        }

        // rotate left
        if (inputs.left)
        {
            double oldDirX = dirX;
            dirX = dirX * cos(rotateSpeed) - dirY * sin(rotateSpeed);
            dirY = oldDirX * sin(rotateSpeed) + dirY * cos(rotateSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotateSpeed) - planeY * sin(rotateSpeed);
            planeY = oldPlaneX * sin(rotateSpeed) + planeY * cos(rotateSpeed);
        }

        // rotate right
        if (inputs.right)
        {
            double oldDirX = dirX;
            dirX = dirX * cos(-rotateSpeed) - dirY * sin(-rotateSpeed);
            dirY = oldDirX * sin(-rotateSpeed) + dirY * cos(-rotateSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotateSpeed) - planeY * sin(-rotateSpeed);
            planeY = oldPlaneX * sin(-rotateSpeed) + planeY * cos(-rotateSpeed);
        }
    }

    CloseWindow();

    return 0;
}