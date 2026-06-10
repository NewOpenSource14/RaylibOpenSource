#include "raylib.h"
#include "../../header/player.h"
#include "../../header/map.h"
#include "../../header/enemy.h"
#include "../../header/close_enemy.h"
#include "../../header/boss.h"

#define MAP_WIDTH  30  
#define MAP_HEIGHT 20  

#define TILE_EMPTY   0
#define TILE_WALL    1
#define TILE_ENEMY   2
#define TILE_ITEM    3
#define TILE_SHUTTER 4
#define TILE_PILLAR  5  
#define TILE_DANGER  6  
#define TILE_CLOSE_ENEMY 7
#define TILE_BOSS    8
#define TILE_GOAL    9

bool isShutterOpen = false;

// 전역 변수 참조
extern CloseEnemy closeEnemy; 
extern Boss boss; 

int const myNewMap[MAP_HEIGHT][MAP_WIDTH] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,2,0,0,0,1,0,0,0,0,0,1,0,0,3,0,0,0,0,0,1},
    {1,1,1,1,1,0,1,0,1,1,1,1,1,7,1,0,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,8,0,0,0,0,0,0,1,0,1},
    {1,0,2,0,1,0,3,0,0,0,0,0,1,0,2,0,1,0,0,0,3,0,0,0,2,0,0,1,0,1},
    {1,0,0,0,1,0,0,0,5,0,0,0,1,0,0,0,1,0,5,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,4,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1},
    {1,0,1,3,0,7,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,1,6,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,6,0,0,0,0,3,0,1},
    {1,0,2,0,1,1,1,1,1,0,1,0,2,0,5,0,2,0,1,0,1,1,1,1,1,0,2,0,0,1},
    {1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,1,4,1,1,1,4,1,1,0,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,5,0,0,0,0,0,0,0,2,0,0,9,0,0,2,0,0,0,0,0,0,0,0,0,0,5,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void EnemeyPlayerSpawnPoint() {
    bool playerSpawned = false;
    bool enemySpawned = false;
    bool closeEnemySpawned = false;
    bool bossSpawned = false; 

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            float worldX = mapPosition.x + (x * WORLD_SCALE) + (WORLD_SCALE / 2.0f);
            float worldZ = mapPosition.z + (y * WORLD_SCALE) + (WORLD_SCALE / 2.0f);

            if (myNewMap[y][x] == TILE_EMPTY && !playerSpawned) {
                player.position = (Vector3){ worldX, 0.0f, worldZ };
                playerSpawned = true;
            }
            else if (myNewMap[y][x] == TILE_ENEMY && !enemySpawned) {
                InitEnemy(&enemy, (Vector3){ worldX, 1.5f, worldZ });
                enemySpawned = true;
            }
            else if (myNewMap[y][x] == TILE_CLOSE_ENEMY && !closeEnemySpawned) {
                InitCloseEnemy(&closeEnemy, (Vector3){ worldX, 0.0f, worldZ });
                closeEnemySpawned = true;
            }
            else if (myNewMap[y][x] == TILE_BOSS && !bossSpawned) {
                InitBoss(&boss, (Vector3){ worldX, 0.0f, worldZ });
                bossSpawned = true;
            }
        }
    }
}

void MapRender() {
    Vector3 floorCenter = { 
        mapPosition.x + (MAP_WIDTH * WORLD_SCALE) / 2.0f, 
        mapPosition.y, 
        mapPosition.z + (MAP_HEIGHT * WORLD_SCALE) / 2.0f 
    };
    DrawPlane(floorCenter, (Vector2){ MAP_WIDTH * WORLD_SCALE, MAP_HEIGHT * WORLD_SCALE }, DARKGRAY);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Vector3 wallPos = { 
                mapPosition.x + x * WORLD_SCALE + (WORLD_SCALE / 2.0f), 
                mapPosition.y, 
                mapPosition.z + y * WORLD_SCALE + (WORLD_SCALE / 2.0f) 
            };

            switch (myNewMap[y][x]) {
                case TILE_WALL: 
                    wallPos.y = mapPosition.y + (3.5f * WORLD_SCALE / 2.0f); 
                    DrawCube(wallPos, WORLD_SCALE, 3.5f * WORLD_SCALE, WORLD_SCALE, GRAY);             
                    DrawCubeWires(wallPos, WORLD_SCALE, 3.5f * WORLD_SCALE, WORLD_SCALE, DARKGRAY);    
                    break;
                case TILE_ENEMY: 
                    wallPos.y = mapPosition.y + 1.5f;
                    DrawCylinder(wallPos, 0.5f, 0.5f, 3.0f, 8, PURPLE);
                    DrawCylinderWires(wallPos, 0.5f, 0.5f, 3.0f, 8, DARKPURPLE);
                    break;
                case TILE_CLOSE_ENEMY:
                    wallPos.y = mapPosition.y + 1.5f;
                    DrawCylinder(wallPos, 0.5f, 0.5f, 3.0f, 8, ORANGE);
                    DrawCylinderWires(wallPos, 0.5f, 0.5f, 3.0f, 8, DARKBROWN);
                    break;
                case TILE_BOSS:
                    wallPos.y = mapPosition.y + 1.5f;
                    //DrawSphere(wallPos, 1.0f, PURPLE);
                    //DrawSphereWires(wallPos, 1.0f, 8, 8, DARKPURPLE);
                    break;
                case TILE_ITEM: 
                    wallPos.y = mapPosition.y + 0.6f;
                    DrawCube(wallPos, 0.8f, 0.8f, 0.8f, YELLOW);             
                    DrawCubeWires(wallPos, 0.8f, 0.8f, 0.8f, GOLD);    
                    break;
                case TILE_SHUTTER: 
                    wallPos.y = mapPosition.y + (3.5f * WORLD_SCALE / 2.0f) + (shutterOpenTimer * 4.5f * WORLD_SCALE); 
                    DrawCube(wallPos, WORLD_SCALE * 0.96f, 3.5f * WORLD_SCALE, 0.25f, BROWN);
                    DrawCubeWires(wallPos, WORLD_SCALE * 0.96f, 3.5f * WORLD_SCALE, 0.25f, DARKBROWN);
                    break;
                case TILE_GOAL: 
                    wallPos.y = mapPosition.y + 0.05f;
                    DrawCube(wallPos, WORLD_SCALE, 0.1f, WORLD_SCALE, LIME);             
                    break;
            }
        }
    }
}