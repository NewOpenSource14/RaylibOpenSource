#include "../../header/userInterface.h"
#include "../../header/shootingLogic.h"
#include "../../header/main.h"
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

// 플레이어 인터페이스 및 보스 체력 바 그리기
void basicPlayerInterface(struct playerInterfaceInfo playerInfo) {
    playerInfo.health = player.health;
    playerInfo.ammo = total_bullets;

    // 플레이어 기본 정보
    DrawText(TextFormat("Health: %d", playerInfo.health), 10, 40, 20, BLUE);
    DrawText(TextFormat("Ammo: %d", playerInfo.ammo), 10, 80, 20, BLUE);
    
    // 적 AI 상태 표시
    DrawText("Enemy AI Active", 10, 100, 20, BLUE);
    DrawText(TextFormat("Enemy Pos: %.2f %.2f %.2f", enemy.position.x, enemy.position.y, enemy.position.z), 10, 120, 20, BLACK);

    // 💡 보스 체력 바 (화면 상단 중앙)
    if (boss.active) {
        int barWidth = 300;
        int barHeight = 20;
        int startX = GetScreenWidth() / 2 - barWidth / 2;
        
        DrawText("BOSS HEALTH", startX, 20, 20, RED);
        DrawRectangle(startX, 45, barWidth, barHeight, DARKGRAY);
        // 체력 비율에 따른 길이 계산 (최대 체력 500 가정)
        float healthPercent = (boss.health > 0) ? (boss.health / 500.0f) : 0.0f;
        DrawRectangle(startX, 45, (int)(healthPercent * barWidth), barHeight, RED);
    }
}

// 미니맵 그리기
void SpawnMiniMap() {
    int scale = 12; 
    int minimapX = GetScreenWidth() - (MAP_WIDTH * scale) - 30;
    int minimapY = 30;

    // 미니맵 배경
    DrawRectangle(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, Fade(BLACK, 0.5f));
    
    // 맵 타일 렌더링
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tile = myNewMap[y][x];
            if (tile == TILE_WALL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, DARKGRAY);
            else if (tile == TILE_SHUTTER) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BROWN);
            else if (tile == TILE_ITEM) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, YELLOW);
            else if (tile == TILE_ENEMY) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, PURPLE); 
            else if (tile == 7) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, Fade(ORANGE, 0.3f));
            else if (tile == TILE_GOAL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, LIME);   
        }
    }
    DrawRectangleLines(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, GREEN); 
    
    // 플레이어 위치
    int playerCellX = (int)((player.position.x - mapPosition.x) / WORLD_SCALE);
    int playerCellY = (int)((player.position.z - mapPosition.z) / WORLD_SCALE);
    if (playerCellX >= 0 && playerCellX < MAP_WIDTH && playerCellY >= 0 && playerCellY < MAP_HEIGHT) {
        DrawRectangle(minimapX + playerCellX * scale, minimapY + playerCellY * scale, scale, scale, GREEN);
    }

    // 원거리 적 위치
    if (enemy.active) {
        int enemyCellX = (int)((enemy.position.x - mapPosition.x) / WORLD_SCALE);
        int enemyCellY = (int)((enemy.position.z - mapPosition.z) / WORLD_SCALE);
        if (enemyCellX >= 0 && enemyCellX < MAP_WIDTH && enemyCellY >= 0 && enemyCellY < MAP_HEIGHT) {
            DrawRectangle(minimapX + enemyCellX * scale, minimapY + enemyCellY * scale, scale, scale, BLUE);
        }
    }

    // 근거리 적 위치
    if (closeEnemy.active) {
        int closeEnemyCellX = (int)((closeEnemy.position.x - mapPosition.x) / WORLD_SCALE);
        int closeEnemyCellY = (int)((closeEnemy.position.z - mapPosition.z) / WORLD_SCALE);
        if (closeEnemyCellX >= 0 && closeEnemyCellX < MAP_WIDTH && closeEnemyCellY >= 0 && closeEnemyCellY < MAP_HEIGHT) {
            DrawRectangle(minimapX + closeEnemyCellX * scale, minimapY + closeEnemyCellY * scale, scale, scale, ORANGE);
        }
    }

    // 💡 보스 위치 (미니맵)
    if (boss.active) {
        int bossCellX = (int)((boss.position.x - mapPosition.x) / WORLD_SCALE);
        int bossCellY = (int)((boss.position.z - mapPosition.z) / WORLD_SCALE);
        if (bossCellX >= 0 && bossCellX < MAP_WIDTH && bossCellY >= 0 && bossCellY < MAP_HEIGHT) {
            DrawRectangle(minimapX + bossCellX * scale, minimapY + bossCellY * scale, scale + 2, scale + 2, DARKPURPLE);
        }
    }
}

void Interface() {
    basicPlayerInterface((struct playerInterfaceInfo){0});
    SpawnMiniMap();
}