#include "raymath.h"
#include <stdbool.h>

extern int spawnPointCount;
extern int totalKilledEnemies;
extern Vector3 enemySpawnPoints[20];
extern float spawnTimer;
extern bool isBossSpawned;

void EnemyManager();
void InitEnemyManager();
