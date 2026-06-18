#include "../../header/gameLogic.h"
#include "../../header/player.h"
#include "../../header/map.h"
#include "../../header/shootingLogic.h"
#include "../../header/enemyManager.h"
#include "../../header/close_enemy.h"
#include "../../header/boss.h"
#include "header/camera.h"

#include "raylib.h"
#include <stdbool.h>

void InitEnemies(void)
{
    for (int i = 0; i < 1000; i++)
    {
        enemies[i].active = false;
        enemies[i].health = 100.0f; 
        enemies[i].position = (Vector3){ 0.0f, 0.0f, 0.0f };
        enemies[i].knockback = (Vector3){ 0.0f, 0.0f, 0.0f };
        enemies[i].hitFlashTimer = 0.0f;
    }
}

void InitCloseEnemies(void)
{
    for (int i = 0; i < 1000; i++)
    {
        closeEnemy[i].active = false;
        closeEnemy[i].health = 100.0f;
        closeEnemy[i].position = (Vector3){ 0.0f, 0.0f, 0.0f };
        closeEnemy[i].knockback = (Vector3){ 0.0f, 0.0f, 0.0f };
        closeEnemy[i].hitFlashTimer = 0.0f;
    }
}

void resetGame(Camera3D *cam) // 카메라 리셋을 위해 포인터로 받으면 좋음
{
    player.health = 100;
    player.position = GetPlayerStartPosition();

    isBossSpawned = false;
    finalBoss.active = false;

    InitShooting();
    
    InitEnemies();
    InitCloseEnemies();
    
    spawnTimer = 0.0f;// 적 스폰 타이머 리셋
    
    cam->position = (Vector3){ player.position.x, player.position.y + 2.0f, player.position.z + 5.0f };
    cam->target = player.position;
    cam->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    totalKilledEnemies = 0;
    
}

void gameLogic(enum GameScreen *screen){
    int framesCounter = 0;
    switch (*screen)
    {
	case GAMEPLAY:
	{
	    if (player.health <= 0)
	    {
		*screen = ENDING;
	    }
	    else if (isBossSpawned && !finalBoss.active)
	    {
		*screen = VICTORY;
	    }
	} break;
	case ENDING:
	{
	    if (IsKeyPressed(KEY_ENTER))
	    {
		resetGame(&camera);
		*screen = GAMEPLAY;
	    }
	} break;
	case VICTORY:
	{
	    if (IsKeyPressed(KEY_ENTER))
	    {
		resetGame(&camera);
		*screen = GAMEPLAY;
	    }
	} break;
	default: break;
    }
}

