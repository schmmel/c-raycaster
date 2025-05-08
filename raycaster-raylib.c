#include "raylib.h"

int main(void)
{
    InitWindow(640, 480, "raycaster");

    while (!WindowShouldClose())
    {
        SetWindowSize(GetScreenWidth() + 1, 480);
        BeginDrawing();
            ClearBackground(WHITE);
            DrawText("a", 190, 200, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}