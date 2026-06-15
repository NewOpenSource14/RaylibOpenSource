#include "raylib.h"
#include "../header/gameLoop.h"

int main(void)
{
    InitGameLoop();

    while (!WindowShouldClose())
    {
        UpdateGameLoop();
        DrawGameLoop();
    }

    CloseGameLoop();

    return 0;
}
