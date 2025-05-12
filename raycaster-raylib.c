#include "raylib.h"

#define screenWidth 640
#define screenHeight 480

int main(void)
{
    InitWindow(screenWidth, screenHeight, "gaycaster");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            
        ClearBackground(WHITE);

        for (int i = 0; i <= screenWidth; i++)
        {
            DrawLine(i, 0, i, screenHeight, ColorFromHSV(i,1,1));
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}