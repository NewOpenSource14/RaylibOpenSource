#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>

/*
    맵 크기와 월드 스케일 정의
    최종 맵은 30 x 20 구조이다.
*/
#define MAP_WIDTH        30
#define MAP_HEIGHT       20
#define WORLD_SCALE      2.5f

/*
    타일 번호 정의
    myNewMap 배열 안의 숫자가 어떤 오브젝트인지 구분한다.
*/
#define TILE_EMPTY          0
#define TILE_WALL           1
#define TILE_ENEMY          2
#define TILE_ITEM           3
#define TILE_SHUTTER        4
#define TILE_WALL_DECOR     5
#define TILE_WALL_DARK      6
#define TILE_GOAL           9
#define TILE_PILLAR         10
#define TILE_COVER          11
#define TILE_EXIT_SIGN      12
#define TILE_WALL_BLUE      13
#define TILE_COLUMN_CYAN    14
#define TILE_DESK           15
#define TILE_TABLE          16
#define TILE_WALL_FLAG      17
#define TILE_BARREL         18
#define TILE_LAMP           19
#define TILE_PORTRAIT       20
#define TILE_PLAYER_START   21

/*
    map_update.c에 실제로 정의되어 있는 전역 변수들.
    다른 파일에서 사용하려면 여기서 extern으로 알려줘야 한다.
*/
extern Vector3 mapPosition;
extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];

extern bool isShutterOpen;
extern float shutterOpenTimer;
extern float shutterHoldTimer;

/*
    맵 위치 / 타일 조회 함수
*/
Vector3 GetMapPosition(void);
int GetMapTileSafe(int x, int y);
Vector3 GetPlayerStartPosition(void);

/*
    플레이어와 맵 충돌 / 바닥 높이 판정 함수
*/
bool CheckMapCollision(Vector3 testPos, float radius);
float GetMapFloorHeight(Vector3 testPos, float radius);

/*
    맵 렌더링 함수
*/
void DrawMap(void);
void MapRender(void);

/*
    2D 미니맵 함수.
    단, 현재 원하는 색깔 미니맵은 userInterface.c의 SpawnMiniMap() 쪽이 담당한다.
    이 함수는 기존 코드 호환용으로 선언만 유지한다.
*/
void DrawMiniMap(Vector3 playerPosition);

/*
    플레이어 / 적 스폰 함수.
    EnemeyPlayerSpawnPoint는 기존 코드의 오타 이름이고,
    EnemyPlayerSpawnPoint는 오타 없는 이름이다.
    둘 다 map_update.c에 있어야 한다.
*/
void EnemeyPlayerSpawnPoint(void);
void EnemyPlayerSpawnPoint(void);

#endif