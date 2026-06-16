#include "../../header/shootingLogic.h"
#include "../../header/enemy.h"

#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdbool.h>

#define MAX_BULLETS 10

static Bullet bullets[MAX_BULLETS] = { 0 };
int total_bullets = MAX_BULLETS;

void InitShooting(void)
{
    total_bullets = MAX_BULLETS;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].position = (Vector3){ 0.0f, 0.0f, 0.0f };
        bullets[i].velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
        bullets[i].radius = 0.1f;
        bullets[i].active = false;
    }
}

void spawnBullet(Camera3D cam)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].position = cam.position;

            Vector3 direction = Vector3Normalize(
                Vector3Subtract(cam.target, cam.position)
            );

            float speed = 50.0f;

            bullets[i].velocity = Vector3Scale(direction, speed);
            bullets[i].radius = 0.1f;
            bullets[i].active = true;

            if (total_bullets > 0)
            {
                total_bullets--;
            }

            return;
        }
    }
}

static bool CheckBulletHitEnemy(Vector3 bulletPos, Enemy *target)
{
    if (target == NULL)
    {
        return false;
    }

    if (!target->active)
    {
        return false;
    }

    Vector2 bullet2D = { bulletPos.x, bulletPos.z };
    Vector2 enemy2D = { target->position.x, target->position.z };

    if (CheckCollisionCircles(bullet2D, 0.1f, enemy2D, 0.6f))
    {
        if (bulletPos.y >= 0.0f && bulletPos.y <= 3.0f)
        {
            return true;
        }
    }

    return false;
}

void ShootingLogic(void)
{
    /*
        현재 구조에서는 gameLoop.c에서 spawnBullet(camera)를 직접 호출한다.
        이 함수는 남겨두지만, 여기서는 아무 것도 하지 않는다.
        이유: shootingLogic.c가 다른 전역 camera를 보면 총알 방향이 어긋날 수 있다.
    */
}

void UpdateAndDrawBullets(Enemy *target)
{
    float deltaTime = GetFrameTime();

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            continue;
        }

        bullets[i].position = Vector3Add(
            bullets[i].position,
            Vector3Scale(bullets[i].velocity, deltaTime)
        );

        if (CheckBulletHitEnemy(bullets[i].position, target))
        {
            printf("Enemy hit\n");

            target->health -= 34.0f;

            if (target->health <= 0.0f)
            {
                target->active = false;
            }

            bullets[i].active = false;
            continue;
        }

        DrawSphere(bullets[i].position, bullets[i].radius, YELLOW);

        /*
            너무 멀리 간 총알은 제거한다.
            기존 코드는 전역 camera를 참조해서 방향/거리 문제가 생길 수 있었다.
        */
        if (Vector3Length(bullets[i].position) > 500.0f)
        {
            bullets[i].active = false;
        }
    }
}