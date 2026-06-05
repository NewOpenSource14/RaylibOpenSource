#include <stdio.h>
#include "../../header/fireball.h"
#include "../../header/main.h"

#include "raylib.h"
#include "raymath.h"

#define MAX_FIREBALLS 50

// -----------------------------------------------------------------------------
// 불덩이 배열
// -----------------------------------------------------------------------------
static Fireball fireballs[MAX_FIREBALLS];

// -----------------------------------------------------------------------------
// 불덩이 초기화
// -----------------------------------------------------------------------------
void InitFireballs(void)
{
    for (int i = 0; i < MAX_FIREBALLS; i++)
    {
        fireballs[i].active = false;
    }
}

// -----------------------------------------------------------------------------
// 외부 맵 데이터 사용
// -----------------------------------------------------------------------------
extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];

extern Vector3 mapPosition;

// -----------------------------------------------------------------------------
// 벽 충돌 검사
// -----------------------------------------------------------------------------
static bool CheckFireballWallCollision(Vector3 pos)
{
    int cellX = (int)((pos.x - mapPosition.x) / WORLD_SCALE);
    int cellY = (int)((pos.z - mapPosition.z) / WORLD_SCALE);
    // 맵 밖
    if (
        cellX < 0
        ||
        cellX >= MAP_WIDTH
        ||
        cellY < 0
        ||
        cellY >= MAP_HEIGHT
    )
    {
        return true;
    }

    // 벽 충돌
    if (myNewMap[cellY][cellX] == 1)
    {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// 시야 체크
// 벽 있으면 false
// -----------------------------------------------------------------------------
bool HasLineOfSight(
    Vector3 start,
    Vector3 end
)
{
    Vector3 dir =
        Vector3Subtract(
            end,
            start
        );

    float distance =
        Vector3Length(dir);

    dir =
        Vector3Normalize(dir);

    float step = 0.1f;

    for (
        float t = 0;
        t < distance;
        t += step
    )
    {
        Vector3 checkPos =
        {
            start.x + dir.x * t,
            start.y,
            start.z + dir.z * t
        };

        if (
            CheckFireballWallCollision(
                checkPos
            )
        )
        {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
// 불덩이 생성
// -----------------------------------------------------------------------------
void SpawnFireball(
    Vector3 startPos,
    Vector3 targetPos
)
{
    for (int i = 0; i < MAX_FIREBALLS; i++)
    {
        if (!fireballs[i].active)
        {
            fireballs[i].position =
                startPos;

            Vector3 dir =
                Vector3Subtract(
                    targetPos,
                    startPos
                );

            fireballs[i].direction =
                Vector3Normalize(dir);

            fireballs[i].speed = 8.0f;

            fireballs[i].active = true;

            break;
        }
    }
}
static bool CHeckFireBallHitPlayer(Vector3 fireballPos, Body* target){
    Vector2 bullet2D = { fireballPos.x, fireballPos.z };
    Vector2 enemy2D = { target->position.x, target->position.z };

    if (CheckCollisionCircles(bullet2D, 0.1f, enemy2D, 0.5f)) {
        if (fireballPos.y >= 0.0f && fireballPos.y <= 2.5f) {
            return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// 불덩이 업데이트
// -----------------------------------------------------------------------------
void UpdateFireballs(float deltaTime, Body* player)
{
    for (int i = 0; i < MAX_FIREBALLS; i++)
    {
        if (!fireballs[i].active)
            continue;

        fireballs[i].position.x +=
            fireballs[i].direction.x
            *
            fireballs[i].speed
            *
            deltaTime;

        fireballs[i].position.y +=
            fireballs[i].direction.y
            *
            fireballs[i].speed
            *
            deltaTime;

        fireballs[i].position.z +=
            fireballs[i].direction.z
            *
            fireballs[i].speed
            *
            deltaTime;

        // 벽 충돌 시 제거
        if (
            CheckFireballWallCollision(
                fireballs[i].position
            )
        )
        {
            fireballs[i].active = false;
        }
	if (CHeckFireBallHitPlayer(fireballs[i].position, player)) {
	    printf("player got hit");
	    player->health -= 34;
	    fireballs[i].active = false; 
	    continue;
	}
    }
}

// -----------------------------------------------------------------------------
// 불덩이 그리기
// -----------------------------------------------------------------------------
void DrawFireballs(void)
{
    for (int i = 0; i < MAX_FIREBALLS; i++)
    {
        if (!fireballs[i].active)
            continue;

        DrawSphere(
            fireballs[i].position,
            0.2f,
            ORANGE
        );
    }
}
