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
<<<<<<< HEAD
}
=======
}
>>>>>>> origin/KAN-8-organized
