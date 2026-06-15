#include "../../header/userInterface.h"
#include "../../header/shootingLogic.h"
#include "../../header/main.h"
#include "../../header/enemy.h"
#include "../../header/map.h"
#include "../../header/player.h"

#include "raylib.h"

struct playerInterfaceInfo
{
    int health;
    int ammo;
};

//이거는 플레이어의 인터페이스를 그려주는 함수이다
void basicPlayerInterface(struct playerInterfaceInfo playerInfo){
    playerInfo.health = player.health;
    playerInfo.ammo = total_bullets;
    DrawText(TextFormat("Health: %d", playerInfo.health), 10, 40, 20, BLUE);
    DrawText(TextFormat("Ammo: %d", playerInfo.ammo), 10, 80, 20, BLUE);
    // 적 관련 정보 적기
    DrawText("Enemy AI Active", 10, 100, 20, BLUE);
    DrawText(
	TextFormat(
	    "Enemy Pos: %.2f %.2f %.2f",
	    enemy.position.x,
	    enemy.position.y,
	    enemy.position.z
	),
	10,
	120,
	20,
	BLACK
    );
}

void SpawnMiniMap(){
    // 2D 미니맵 레이더
    int scale = 12; 
    int minimapX = GetScreenWidth() - (MAP_WIDTH * scale) - 30;
    int minimapY = 30;

    DrawRectangle(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, Fade(BLACK, 0.5f));
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
	for (int x = 0; x < MAP_WIDTH; x++) {
	    int tile = myNewMap[y][x];
	    if (tile == TILE_WALL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, DARKGRAY);
	    else if (tile == TILE_SHUTTER) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BROWN);
	    else if (tile == TILE_ITEM) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, YELLOW);
	    else if (tile == TILE_ENEMY) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, PURPLE); 
	    else if (tile == TILE_GOAL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, LIME);   
	}
    }
    DrawRectangleLines(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, GREEN); 
    int playerCellX = (int)((player.position.x - mapPosition.x) / WORLD_SCALE);
    int playerCellY = (int)((player.position.z - mapPosition.z) / WORLD_SCALE);
    if (playerCellX >= 0 && playerCellX < MAP_WIDTH && playerCellY >= 0 && playerCellY < MAP_HEIGHT) {
	DrawRectangle(minimapX + playerCellX * scale, minimapY + playerCellY * scale, scale, scale, GREEN);
    }

    // 미니맵 위 플레이어 위치이다 yes
    //player이 정확의 어디에 위치를 하고 있는지 계산
    //player.x - 전체 map position player 위치만 남는다
    //적 관련 X, Y 좌표를 통해 위치 계산
int enemyCellX = (int)((enemy.position.x - mapPosition.x) / WORLD_SCALE);
int enemyCellY = (int)((enemy.position.z - mapPosition.z) / WORLD_SCALE);
//적 그리기
    DrawRectangle(minimapX + enemyCellX * scale, minimapY + enemyCellY * scale, scale, scale, BLUE);
}

void Interface()
{
    //이거는 그냥 화면에 텍스트를 띄우는 것이다
    DrawRectangle(325, 300, 150, 150, SKYBLUE);
    basicPlayerInterface((struct playerInterfaceInfo){0});
    SpawnMiniMap();
}

