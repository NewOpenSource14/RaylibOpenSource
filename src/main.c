#include "../header/gameLoop.h"
#include "raylib.h"

int main(void)
{
    InitGameLoop();
    while (!WindowShouldClose())
    {
        UpdateGameLoop();
        DrawGameLoop();
    }
    CloseGameLoop();
    CloseWindow();                  
    return 0;
}
