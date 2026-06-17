#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>

#define MAP_WIDTH     30
#define MAP_HEIGHT    20
#define WORLD_SCALE   2.5f

#define TILE_EMPTY         0
#define TILE_WALL          1
#define TILE_ENEMY         2
#define TILE_ITEM          3
#define TILE_SHUTTER       4
#define TILE_WALL_DECOR    5
#define TILE_WALL_DARK     6
#define TILE_GOAL          9
#define TILE_PILLAR        10
#define TILE_COVER         11
#define TILE_EXIT_SIGN     12
#define TILE_WALL_BLUE     13
#define TILE_COLUMN_CYAN   14
#define TILE_DESK          15
#define TILE_TABLE         16
#define TILE_WALL_FLAG     17
#define TILE_BARREL        18
#define TILE_LAMP          19
#define TILE_PORTRAIT      20
#define TILE_PLAYER_START  21

extern Vector3 mapPosition;
extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];

extern bool isShutterOpen;
extern float shutterOpenTimer;
extern float shutterHoldTimer;

Vector3 GetPlayerStartPosition(void);
Vector3 GetMapPosition(void);
int GetMapTileSafe(int x, int y);

bool CheckMapCollision(Vector3 testPos, float radius);
float GetMapFloorHeight(Vector3 testPos, float radius);

void DrawMap(void);
void DrawMiniMap(Vector3 playerPosition);
void MapRender(void);

void EnemeyPlayerSpawnPoint(void);
void EnemyPlayerSpawnPoint(void);

#endif