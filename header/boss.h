#ifndef BOSS_H
#define BOSS_H

#include "raylib.h"

typedef struct {
    Vector3 position;
    Vector3 knockback; // 넛백용 벡터
    float health;
    bool active;
    float timer;       // 패턴 타이머
    int currentPattern;

    bool hitFlash;
    float hitFlashTimer;
} Boss;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    bool active;
    float lifetime;
} Projectile;

// 함수 선언
void InitBoss(Boss* boss, Vector3 spawnPos);
void UpdateBoss(Boss* boss, Vector3 playerPos, float deltaTime);
void DrawBoss(Boss* boss);
void ApplyBossKnockback(Boss* boss, Vector3 force);

#endif