#include "header/map.h"
#include "raylib.h"
#include "raymath.h"
#include "../../header/boss.h"
#include "../../header/main.h"
#include <stdio.h>

#define MAX_BULLETS 16
#define MAX_IMPACTS 32

// =========================
// bullets
// =========================
static Projectile bullets[MAX_BULLETS];

// =========================
// impact effect
// =========================
typedef struct {
    Vector3 position;
    float timer;
    bool active;
} Impact;

static Impact impacts[MAX_IMPACTS];

// =========================
// trap
// =========================
static Vector3 trapPos;
static float trapTimer = 0.0f;

// =========================
// hit flash
// =========================
static float hitFlashTimer = 0.0f;

// =========================
// external map (enemy 방식 그대로)
// =========================
extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];
extern Vector3 mapPosition;

// =========================
// impact spawn
// =========================
static void SpawnImpact(Vector3 pos)
{
    for (int i = 0; i < MAX_IMPACTS; i++) {
        if (!impacts[i].active) {
            impacts[i].position = pos;
            impacts[i].timer = 0.2f;
            impacts[i].active = true;
            return;
        }
    }
}

// =========================
// init
// =========================
void InitBoss(Boss* boss, Vector3 spawnPos)
{
    boss->position = spawnPos;
    boss->knockback = (Vector3){0, 0, 0};
    boss->health = 500.0f;
    boss->active = true;
    boss->timer = 3.0f;
    boss->currentPattern = 0;

    hitFlashTimer = 0.0f;

    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i].active = false;

    for (int i = 0; i < MAX_IMPACTS; i++)
        impacts[i].active = false;
}

// =========================
// knockback (enemy 스타일)
// =========================
void ApplyBossKnockback(Boss* boss, Vector3 force)
{
    boss->knockback = Vector3Add(
        boss->knockback,
        Vector3Scale(force, 0.3f)
    );

    hitFlashTimer = 0.2f;
}

// =========================
// update
// =========================
void UpdateBoss(Boss* boss, Vector3 playerPos, float* playerHealth, float dt)
{
    if (!boss->active) return;

    // =========================
    // hit flash
    // =========================
    if (hitFlashTimer > 0)
        hitFlashTimer -= dt;

    // =========================
    // knockback (X/Z 분리)
    // =========================
    if (Vector3Length(boss->knockback) > 0.05f)
    {
        Vector3 nextX = boss->position;
        nextX.x += boss->knockback.x * dt;

        if (!CheckMapCollision(nextX,0.6f))
            boss->position.x = nextX.x;

        Vector3 nextZ = boss->position;
        nextZ.z += boss->knockback.z * dt;

        if (!CheckMapCollision(nextZ,0.6f))
            boss->position.z = nextZ.z;

        float friction = 1.0f - (8.0f * dt);
        if (friction < 0) friction = 0;

        boss->knockback = Vector3Scale(boss->knockback, friction);
    }
    else
    {
        // =========================
        // chase player (enemy 방식 그대로)
        // =========================
        Vector3 dir = Vector3Subtract(playerPos, boss->position);
        float dist = Vector3Length(dir);

        if (dist > 5.0f)
        {
            dir = Vector3Normalize(dir);

            Vector3 nextX = boss->position;
            nextX.x += dir.x * 1.5f * dt;

            if (!CheckMapCollision(nextX, 0.6f))
                boss->position.x = nextX.x;

            Vector3 nextZ = boss->position;
            nextZ.z += dir.z * 1.5f * dt;

            if (!CheckMapCollision(nextZ, 0.6f))
                boss->position.z = nextZ.z;
        }
    }

    // =========================
    // pattern timer
    // =========================
    boss->timer -= dt;

    if (boss->timer <= 0)
    {
        boss->currentPattern = GetRandomValue(1, 2);

        if (boss->currentPattern == 1)
        {
            for (int i = 0; i < 8; i++)
            {
                float a = (2.0f * PI / 8.0f) * i;

                bullets[i].position = boss->position;
                bullets[i].velocity = (Vector3){
                    cosf(a) * 5.0f,
                    0,
                    sinf(a) * 5.0f
                };

                bullets[i].active = true;
                bullets[i].lifetime = 2.0f;
            }
        }
        else
        {
            trapPos = playerPos;
            trapTimer = 1.0f;
        }

        boss->timer = 3.0f;
    }

    // =========================
    // bullets + wall collision
    // =========================
    for (int i = 0; i < 8; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].position = Vector3Add(
                bullets[i].position,
                Vector3Scale(bullets[i].velocity, dt)
            );
	    Vector2 bullet2D = { bullets[i].position.x, bullets[i].position.z };
	    Vector2 player2D = { playerPos.x, playerPos.z };

	    if (Vector2Distance(bullet2D, player2D) < 1.0f)
	    {
		*playerHealth -= 15.0f;
		bullets[i].active = false;
		printf("보스 총알 맞음 그리고 남은 피: %.1f\n", *playerHealth);
		continue;
	    }

            if (CheckMapCollision(bullets[i].position, 0.6f))
            {
                SpawnImpact(bullets[i].position);
                bullets[i].active = false;
                continue;
            }

            bullets[i].lifetime -= dt;
            if (bullets[i].lifetime <= 0)
                bullets[i].active = false;
        }
    }

    // =========================
    // impacts update
    // =========================
    for (int i = 0; i < MAX_IMPACTS; i++)
    {
        if (impacts[i].active)
        {
            impacts[i].timer -= dt;
            if (impacts[i].timer <= 0)
                impacts[i].active = false;
        }
    }

    if (trapTimer > 0)
        trapTimer -= dt;
}

// =========================
// draw
// =========================
void DrawBoss(Boss* boss)
{
    if (!boss->active) return;

    Vector3 p = boss->position;

    bool hit = (hitFlashTimer > 0);

    Color body = hit ? RED : DARKPURPLE;
    Color wire = hit ? MAROON : BLACK;

    // =========================
    // body (enemy scale 느낌으로 조정)
    // =========================
    DrawCube(p, 1.8f, 2.4f, 1.8f, body);
    DrawCubeWires(p, 1.8f, 2.4f, 1.8f, wire);


    // =========================
    // bullets
    // =========================
    for (int i = 0; i < 8; i++)
        if (bullets[i].active)
            DrawSphere(bullets[i].position, 0.3f, WHITE);

    // =========================
    // impacts
    // =========================
    for (int i = 0; i < MAX_IMPACTS; i++)
    {
        if (impacts[i].active)
        {
            float t = impacts[i].timer / 0.2f;

            DrawSphere(impacts[i].position, 0.4f * (1 - t), Fade(ORANGE, t));
        }
    }
}
