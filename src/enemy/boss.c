#include "raylib.h"
#include "raymath.h"
#include "../../header/boss.h"
#include "../../header/main.h"

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
// enemy 스타일 collision (중요)
// =========================
static bool CheckBossCollision(Vector3 testPos)
{
    Vector2 pos2D = { testPos.x, testPos.z };

    int cellX = (int)((testPos.x - mapPosition.x) / WORLD_SCALE);
    int cellY = (int)((testPos.z - mapPosition.z) / WORLD_SCALE);

    for (int y = cellY - 1; y <= cellY + 1; y++) {
        if (y >= 0 && y < MAP_HEIGHT) {
            for (int x = cellX - 1; x <= cellX + 1; x++) {
                if (x >= 0 && x < MAP_WIDTH) {
                    if (myNewMap[y][x] == 1) {

                        Rectangle wallRect = {
                            mapPosition.x + (x * WORLD_SCALE),
                            mapPosition.z + (y * WORLD_SCALE),
                            WORLD_SCALE,
                            WORLD_SCALE
                        };

                        if (CheckCollisionCircleRec(pos2D, 0.6f, wallRect))
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

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
void UpdateBoss(Boss* boss, Vector3 playerPos, float dt)
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

        if (!CheckBossCollision(nextX))
            boss->position.x = nextX.x;

        Vector3 nextZ = boss->position;
        nextZ.z += boss->knockback.z * dt;

        if (!CheckBossCollision(nextZ))
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

            if (!CheckBossCollision(nextX))
                boss->position.x = nextX.x;

            Vector3 nextZ = boss->position;
            nextZ.z += dir.z * 1.5f * dt;

            if (!CheckBossCollision(nextZ))
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

            if (CheckBossCollision(bullets[i].position))
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