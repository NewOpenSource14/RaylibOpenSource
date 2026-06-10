#include "../../header/enemy.h"
#include "../../header/fireball.h"
#include "../../header/main.h"

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

#define ENEMY_RADIUS 0.3f

// main.c 에 있는 맵 데이터 사용
extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];
extern Vector3 mapPosition;

// -----------------------------------------------------------------------------
// 벽 충돌 검사
// -----------------------------------------------------------------------------
static bool CheckEnemyCollision(Vector3 testPos)
{
    Vector2 pos2D =
    {
        testPos.x,
        testPos.z
    };

    int cellX = (int)((testPos.x - mapPosition.x) / WORLD_SCALE);
    int cellY = (int)((testPos.z - mapPosition.z) / WORLD_SCALE);

    for (int y = cellY - 1; y <= cellY + 1; y++)
    {
        if (y >= 0 && y < MAP_HEIGHT)
        {
            for (int x = cellX - 1; x <= cellX + 1; x++)
            {
                if (x >= 0 && x < MAP_WIDTH)
                {
                    if (myNewMap[y][x] == 1)
                    {
                        Rectangle wallRect =
                        {
                            mapPosition.x + (x * WORLD_SCALE),
                            mapPosition.z + (y * WORLD_SCALE),
                            WORLD_SCALE,
                            WORLD_SCALE
                        };

                        if (
                            CheckCollisionCircleRec(
                                pos2D,
                                ENEMY_RADIUS,
                                wallRect
                            )
                        )
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// 적 초기화
// -----------------------------------------------------------------------------
void InitEnemy(
    Enemy* enemy,
    Vector3 startPos
)
{
    enemy->health = 100;
    enemy->position = startPos;
    enemy->speed = 2.0f;
    enemy->detectRange = 15.0f * WORLD_SCALE;
    enemy->active = true;

    // 추가된 변수 초기화
    enemy->knockback = (Vector3){ 0.0f, 0.0f, 0.0f };
    enemy->hitFlashTimer = 0.0f;
}

// -----------------------------------------------------------------------------
// 적 업데이트
// -----------------------------------------------------------------------------
void UpdateEnemy(
    Enemy* enemy,
    Vector3 playerPos,
    float deltaTime
)
{
    // 죽음 처리 및 색상 강제 초기화 안전장치
    if (enemy->health <= 0)
    {
        enemy->active = false;
        enemy->hitFlashTimer = 0.0f; 
    }

    if (!enemy->active)
        return;

    // 피격 플래시 타이머 업데이트
    if (enemy->hitFlashTimer > 0.0f)
    {
        enemy->hitFlashTimer -= deltaTime;
    }

    // 넛백 처리 (매 프레임 감쇄하며 벽 충돌 검사 후 이동)
    if (Vector3Length(enemy->knockback) > 0.05f)
    {
        // X축 넛백 이동
        Vector3 nextPosX = enemy->position;
        nextPosX.x += enemy->knockback.x * deltaTime;
        if (!CheckEnemyCollision(nextPosX)) enemy->position.x = nextPosX.x;

        // Z축 넛백 이동
        Vector3 nextPosZ = enemy->position;
        nextPosZ.z += enemy->knockback.z * deltaTime;
        if (!CheckEnemyCollision(nextPosZ)) enemy->position.z = nextPosZ.z;

        // 마찰력 적용 (매 초마다 자연스럽게 스르륵 멈춤)
        float friction = 1.0f - (10.0f * deltaTime); 
        if (friction < 0.0f) friction = 0.0f;
        enemy->knockback = Vector3Scale(enemy->knockback, friction);
    }

    // 플레이어 방향 계산
    Vector3 direction =
        Vector3Subtract(
            playerPos,
            enemy->position
        );

    // 거리 계산
    float distance =
        Vector3Length(direction);

    // -------------------------------------------------------------------------
    // 플레이어 감지 시 추적
    // -------------------------------------------------------------------------
    if (distance < enemy->detectRange)
    {
        direction =
            Vector3Normalize(direction);

        // =====================================================
        // X축 이동
        // =====================================================
        Vector3 nextPosX =
            enemy->position;

        nextPosX.x +=
            direction.x
            *
            enemy->speed
            *
            deltaTime;

        if (!CheckEnemyCollision(nextPosX))
        {
            enemy->position.x =
                nextPosX.x;
        }

        // =====================================================
        // Z축 이동
        // =====================================================
        Vector3 nextPosZ =
            enemy->position;

        nextPosZ.z +=
            direction.z
            *
            enemy->speed
            *
            deltaTime;

        if (!CheckEnemyCollision(nextPosZ))
        {
            enemy->position.z =
                nextPosZ.z;
        }
    }

    // -------------------------------------------------------------------------
    // 불덩이 공격
    // -------------------------------------------------------------------------
    static float shootTimer = 0.0f;
    shootTimer += deltaTime;

    if (
        distance < enemy->detectRange
        &&
        shootTimer >= 2.0f
        &&
        HasLineOfSight(
            enemy->position,
            playerPos
        )
    )
    {
        Vector3 fireballStart =
        {
            enemy->position.x,
            enemy->position.y + 1.0f,
            enemy->position.z
        };

        SpawnFireball(
            fireballStart,
            playerPos
        );

        shootTimer = 0.0f;
    }
}

// -----------------------------------------------------------------------------
// 적 렌더링
// -----------------------------------------------------------------------------
void DrawEnemy(Enemy* enemy)
{
    if (!enemy->active)
        return;

    Vector3 drawPos =
        enemy->position;

    drawPos.y = 0.75f;

    // 현재 피격 타이머가 돌아가는 중인지 체크
    bool isHit = (enemy->hitFlashTimer > 0.0f);

    // 피격 여부에 따른 최종 출력 색상 세팅
    Color bodyColor = isHit ? RED : BLUE;
    Color wireColor = isHit ? MAROON : DARKBLUE;
    Color headColor = isHit ? PINK : SKYBLUE; // 머리도 피격 시 빨간색 계열(PINK)로 변경

    // 몸통
    DrawCube(
        drawPos,
        1.0f,
        1.5f,
        1.0f,
        bodyColor
    );

    // 테두리
    DrawCubeWires(
        drawPos,
        1.0f,
        1.5f,
        1.0f,
        wireColor
    );

    // 머리
    DrawSphere(
        (Vector3)
        {
            drawPos.x,
            drawPos.y + 1.1f,
            drawPos.z
        },
        0.25f,
        headColor
    );
}