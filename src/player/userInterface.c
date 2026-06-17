#include "../../header/userInterface.h"
#include "../../header/shootingLogic.h"
#include "../../header/enemy.h"
#include "../../header/close_enemy.h"
#include "../../header/boss.h"
#include "../../header/map.h"
#include "../../header/player.h"

#include "raylib.h"

// 전역 변수 참조
extern CloseEnemy closeEnemy;
extern Boss boss; 

// 플레이어 인터페이스 정보 구조체
struct playerInterfaceInfo {
    int health;
    int ammo;
};

static Color GetMiniMapTileColor(int tile)
{
    switch (tile)
    {
        case TILE_EMPTY:
            return (Color){ 45, 150, 130, 255 };     // 빈 공간: 청록/초록 느낌

        case TILE_WALL:
            return (Color){ 12, 20, 28, 255 };       // 외벽: 거의 검정

        case TILE_WALL_BLUE:
            return (Color){ 0, 92, 210, 255 };       // 파란 벽

        case TILE_WALL_DECOR:
            return (Color){ 120, 20, 20, 255 };      // 장식 벽: 진한 빨강

        case TILE_WALL_DARK:
            return (Color){ 25, 25, 30, 255 };       // 어두운 벽

        case TILE_WALL_FLAG:
            return (Color){ 215, 25, 35, 255 };      // 빨간 장식

        case TILE_PORTRAIT:
            return (Color){ 125, 30, 160, 255 };     // 보라색 장식

        case TILE_GOAL:
            return (Color){ 80, 210, 60, 255 };      // 목표 지점

        case TILE_PILLAR:
            return (Color){ 45, 55, 65, 255 };       // 기둥

        case TILE_COVER:
            return (Color){ 115, 70, 30, 255 };      // 엄폐물: 갈색

        case TILE_EXIT_SIGN:
            return (Color){ 20, 190, 70, 255 };      // 출구: 초록

        case TILE_COLUMN_CYAN:
            return (Color){ 0, 190, 210, 255 };      // 청록 기둥

        case TILE_DESK:
            return (Color){ 100, 55, 25, 255 };      // 책상

        case TILE_TABLE:
            return (Color){ 150, 85, 35, 255 };      // 테이블

        case TILE_BARREL:
            return (Color){ 30, 130, 55, 255 };      // 초록 배럴

        case TILE_LAMP:
            return (Color){ 255, 215, 0, 255 };      // 램프: 노랑

        case TILE_ENEMY:
            return (Color){ 225, 20, 30, 255 };      // 적: 빨강

        case TILE_ITEM:
            return (Color){ 255, 205, 0, 255 };      // 아이템: 노랑

        case TILE_PLAYER_START:
            return (Color){ 25, 190, 70, 255 };      // 시작 지점: 초록

        case TILE_SHUTTER:
            return (Color){ 130, 75, 35, 255 };      // 셔터

        default:
            return (Color){ 45, 150, 130, 255 };
    }
}

static void DrawPlayerInterface(struct playerInterfaceInfo playerInfo)
{
    playerInfo.health = (int)player.health;
    playerInfo.ammo = total_bullets;

    DrawText(TextFormat("Health: %d", playerInfo.health), 10, 40, 20, BLUE);
    DrawText(TextFormat("Ammo: %d", playerInfo.ammo), 10, 70, 20, BLUE);

    if (enemy.active)
    {
        DrawText("Enemy AI Active", 10, 100, 20, BLUE);
    }
    else
    {
        DrawText("Enemy Down", 10, 100, 20, RED);
    }
}

void SpawnMiniMap(void)
{
    // 2D 미니맵 레이더
    // 기존보다 크게 보이도록 scale을 8로 설정한다.
    int scale = 12;

    int minimapX = GetScreenWidth() - (MAP_WIDTH * scale) - 20;
    int minimapY = 20;

    DrawRectangle(
        minimapX,
        minimapY,
        MAP_WIDTH * scale,
        MAP_HEIGHT * scale,
        Fade(BLACK, 0.5f)
    );

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int tile = myNewMap[y][x];
            Color color = BLANK;

            if (tile == TILE_EMPTY) color = (Color){ 240, 235, 220, 255 };
            else if (tile == TILE_WALL) color = DARKGRAY;
            else if (tile == TILE_WALL_DECOR) color = MAROON;
            else if (tile == TILE_WALL_DARK) color = BLACK;
            else if (tile == TILE_GOAL) color = LIME;
            else if (tile == TILE_PILLAR) color = DARKGRAY;
            else if (tile == TILE_COVER) color = BROWN;
            else if (tile == TILE_EXIT_SIGN) color = GREEN;
            else if (tile == TILE_WALL_BLUE) color = BLUE;
            else if (tile == TILE_COLUMN_CYAN) color = SKYBLUE;
            else if (tile == TILE_DESK) color = BROWN;
            else if (tile == TILE_TABLE) color = DARKBROWN;
            else if (tile == TILE_WALL_FLAG) color = RED;
            else if (tile == TILE_BARREL) color = DARKGREEN;
            else if (tile == TILE_LAMP) color = GREEN;
            else if (tile == TILE_PORTRAIT) color = GOLD;
            else if (tile == TILE_ENEMY) color = RED;
            else if (tile == TILE_PLAYER_START) color = GREEN;

            /*
                노란 아이템은 지워달라고 했으므로 TILE_ITEM은 일부러 그리지 않는다.
                아이템을 다시 보이게 하고 싶으면 아래 줄을 추가하면 된다.
                else if (tile == TILE_ITEM) color = ORANGE;
            */

            if (color.a > 0)
            {
                DrawRectangle(
                    minimapX + x * scale,
                    minimapY + y * scale,
                    scale,
                    scale,
                    color
                );

                DrawRectangleLines(
                    minimapX + x * scale,
                    minimapY + y * scale,
                    scale,
                    scale,
                    Fade(LIGHTGRAY, 0.35f)
                );
            }
        }
    }

    // 맵 테두리
    DrawRectangleLines(
        minimapX,
        minimapY,
        MAP_WIDTH * scale,
        MAP_HEIGHT * scale,
        GREEN
    );

    // 플레이어 위치
    int playerCellX = (int)((player.position.x - mapPosition.x) / WORLD_SCALE);
    int playerCellY = (int)((player.position.z - mapPosition.z) / WORLD_SCALE);

    if (playerCellX >= 0 && playerCellX < MAP_WIDTH &&
        playerCellY >= 0 && playerCellY < MAP_HEIGHT)
    {
        DrawRectangle(
            minimapX + playerCellX * scale,
            minimapY + playerCellY * scale,
            scale,
            scale,
            LIME
        );

        DrawRectangleLines(
            minimapX + playerCellX * scale,
            minimapY + playerCellY * scale,
            scale,
            scale,
            WHITE
        );
    }

    // 실제 적 위치
    if (enemy.active)
    {
        int enemyCellX = (int)((enemy.position.x - mapPosition.x) / WORLD_SCALE);
        int enemyCellY = (int)((enemy.position.z - mapPosition.z) / WORLD_SCALE);

        if (enemyCellX >= 0 && enemyCellX < MAP_WIDTH &&
            enemyCellY >= 0 && enemyCellY < MAP_HEIGHT)
        {
            DrawRectangle(
                minimapX + enemyCellX * scale,
                minimapY + enemyCellY * scale,
                scale,
                scale,
                RED
            );

            DrawRectangleLines(
                minimapX + enemyCellX * scale,
                minimapY + enemyCellY * scale,
                scale,
                scale,
                YELLOW
            );
        }
    }
}

void Interface(void)
{
    DrawPlayerInterface((struct playerInterfaceInfo){ 0 });
    SpawnMiniMap();
}