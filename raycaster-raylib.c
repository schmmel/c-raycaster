#include "raylib.h"
#include <math.h>

#define screenWidth 640
#define screenHeight 480

#define mapHeight 8
#define mapWidth 8

int map[mapHeight][mapWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 2, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 2, 1},
    {1, 2, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

int wallColors[][3] = {
    {128, 0, 128},
    {223, 12, 68},
};

// w, a, s, d, arrow left, arrow right
int inputs[] = {0, 0, 0, 0, 0, 0};

int main(void)
{
    double playerX = 4.5, playerY = 2.5;
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;

    InitWindow(screenWidth, screenHeight, "raycaster");

    while (!WindowShouldClose())
    {
        inputs[0] = IsKeyDown(KEY_W) ? 1 : 0;
        inputs[1] = IsKeyDown(KEY_A) ? 1 : 0;
        inputs[2] = IsKeyDown(KEY_S) ? 1 : 0;
        inputs[3] = IsKeyDown(KEY_D) ? 1 : 0;
        inputs[4] = IsKeyDown(KEY_LEFT) ? 1 : 0;
        inputs[5] = IsKeyDown(KEY_RIGHT) ? 1 : 0;

        BeginDrawing();

        ClearBackground(BLANK);

        for (int x = 0; x <= screenWidth; x++)
        {
            double cameraX = 2 * x / (double)screenWidth - 1;
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

            DrawLine(x, drawStart, x, drawEnd, (Color){color[0], color[1], color[2], 255});
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}