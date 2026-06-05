#pragma once
#include "raylib.h"

#include "main.h"

extern Vector3 mapPosition;
extern const int myNewMap[MAP_HEIGHT][MAP_WIDTH];
extern float shutterOpenTimer;
extern float shutterHoldTimer;
extern bool isShutterOpen;

//함수
void EnemeyPlayerSpawnPoint();
void MapRender();
