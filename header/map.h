#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>

// 맵 크기와 월드 스케일을 정의하는 부분이다.
// 기존 팀원 코드는 16x16 맵이었지만, 맵디자인 최종본은 30x20 구조로 확장한다.
#define MAP_WIDTH        30
#define MAP_HEIGHT       20
#define WORLD_SCALE      2.5f

// 타일 번호 정의
// 맵 요소를 숫자 타일로 구분한다.
// 배열 값만 바꿔도 맵 구조와 오브젝트 배치를 수정할 수 있다.
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

// 플레이어 시작 위치를 맵 배열에서 찾아서 반환하는 함수이다.
Vector3 GetPlayerStartPosition(void);

// 맵 전체의 중심 위치를 반환하는 함수이다.
Vector3 GetMapPosition(void);

// 맵 타일을 안전하게 가져오는 함수이다.
// 맵 밖을 참조하면 벽으로 처리한다.
int GetMapTileSafe(int x, int y);

// 플레이어와 맵의 충돌을 확인하는 함수이다.
bool CheckMapCollision(Vector3 testPos, float radius);

// 3D 맵을 그리는 함수이다.
void DrawMap(void);

// 2D 미니맵을 그리는 함수이다.
void DrawMiniMap(Vector3 playerPosition);

#endif