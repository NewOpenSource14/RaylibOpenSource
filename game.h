#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

#define GRAVITY         32.0f
#define MAX_SPEED       12.0f
#define CROUCH_SPEED     6.0f
#define JUMP_FORCE      12.0f
#define MAX_ACCEL       90.0f
#define FRICTION         0.86f
#define AIR_DRAG         0.98f
#define CONTROL          15.0f
#define CROUCH_HEIGHT    0.0f
#define STAND_HEIGHT     1.2f
#define BOTTOM_HEIGHT    0.6f

#define MAP_WIDTH        30
#define MAP_HEIGHT       20
#define WORLD_SCALE      2.5f

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

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 dir;
    bool isGrounded;
} Body;

extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];

#endif