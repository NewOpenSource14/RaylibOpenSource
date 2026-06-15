#pragma once

#include "raylib.h"
//맵 관리
//------------------------
#define MAP_WIDTH  30  
#define MAP_HEIGHT 20  
#define WORLD_SCALE 2.5f
#define TILE_EMPTY   0  
#define TILE_WALL    1  
#define TILE_ENEMY   2  
#define TILE_ITEM    3  
#define TILE_SHUTTER 4  
#define TILE_GOAL    9
//------------------------

//플레이러 대한 define
//-----------------------------
#define STAND_HEIGHT	1.0f
#define GRAVITY         32.0f
#define MAX_SPEED       20.0f
#define CROUCH_SPEED     5.0f
#define JUMP_FORCE      12.0f
#define MAX_ACCEL      150.0f
#define FRICTION         0.86f
#define AIR_DRAG         0.98f
#define CONTROL         15.0f
#define CROUCH_HEIGHT    0.0f
#define BOTTOM_HEIGHT    0.5f

#define NORMALIZE_INPUT  0
//------------------------------

//맵 관리 변수들이다.
//------------------------
#define MAP_WIDTH  30  
#define MAP_HEIGHT 20  
#define WORLD_SCALE 2.5f 
//------------------------
//extern을 붙이면 중복 정의 에러가 막을 수 있음
extern const int screenWidth;
extern const int screenHeight;
