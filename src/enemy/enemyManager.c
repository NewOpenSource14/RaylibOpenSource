#include "../../header/boss.h"
#include "../../header/close_enemy.h"
#include "../../header/enemy.h"
#include "../../header/enemyManager.h"
#include "../../header/player.h"
#include "../../header/gameLoop.h"
#include "raylib.h"

CloseEnemy closeEnemy[1000];
Enemy enemies[1000];
Boss finalBoss;
bool isBossSpawned = false;

Vector3 enemySpawnPoints[20];
int spawnPointCount = 0;
float spawnTimer = 0;

float spawnInterval = 3.0f; 
int totalKilledEnemies = 0;

void SpawnEnemyAtRandomPoint(void)
{
    if (spawnPointCount == 0) return; 
    int randIndex = GetRandomValue(0, spawnPointCount - 1);
    Vector3 spawnPos = enemySpawnPoints[randIndex];

    int enemyType = GetRandomValue(0, 1);

    if (enemyType == 0)
    {
        for (int i = 0; i < 1000; i++)
        {
            if (!enemies[i].active)
            {
                InitEnemy(&enemies[i], spawnPos);
                enemies[i].active = true; 
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 1000; i++)
        {
            if (!closeEnemy[i].active)
            {
                InitCloseEnemy(&closeEnemy[i], spawnPos);
                closeEnemy[i].active = true;
                break;
            }
        }
    }
    if (totalKilledEnemies >= 1 && !isBossSpawned)
    {
        int centerRow = 9;
        int centerCol = 14;
        
        float tileSize = 1.0f; 
        
        Vector3 bossSpawnPos = { 0.0f, 0.0f, 0.0f };

        bossSpawnPos.x = centerCol * tileSize;
        bossSpawnPos.y = 1.0f;
        bossSpawnPos.z = centerRow * tileSize;


        InitBoss(&finalBoss, bossSpawnPos);
        isBossSpawned = true;
    }
}
void InitEnemyManager(void) 
{
    spawnTimer = 0.0f; 
}

void EnemyManager(void)
{
    spawnTimer += GetFrameTime();
    if (spawnTimer >= spawnInterval)
    {
        SpawnEnemyAtRandomPoint();
        spawnTimer = 0.0f;
    }


    for (int i = 0; i < 1000; i++)
    {
        if (enemies[i].active)
        {
            UpdateEnemy(&enemies[i], player.position, delta);
        }
    }
    for (int i = 0; i < 1000; i++)
    {
        if (closeEnemy[i].active)
        {
            // close_enemy.h에 정의되어 있을 업데이트 함수 호출
            UpdateCloseEnemy(&closeEnemy[i], player.position, &player.health, delta);
        }
    }

    if (isBossSpawned && finalBoss.active)
    {
        UpdateBoss(&finalBoss, player.position, &player.health, delta);
    }
}
