#pragma once

#include "enemy.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Vector3 position;
    Vector3 velocity;
    float radius;
    bool active;
} Bullet;

extern int total_bullets;

void InitShooting(void);
void spawnBullet(Camera3D cam);
void ShootingLogic(void);
void UpdateAndDrawBullets(Enemy* target);