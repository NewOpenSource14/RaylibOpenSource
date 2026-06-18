#include "../../header/shootingLogic.h"
#include "../../header/enemy.h"
#include "../../header/boss.h"
#include "../../header/enemyManager.h"
#include "header/close_enemy.h"
#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdbool.h>

#define MAX_BULLETS 200

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

static bool CheckBulletHitEntity(Vector3 bulletPos, Vector3 entityPos, float entityRadius) {
    Vector2 bullet2D = { bulletPos.x, bulletPos.z };
    Vector2 enemy2D = { entityPos.x, entityPos.z };

    if (CheckCollisionCircles(bullet2D, 0.1f, enemy2D, entityRadius)) {
        if (bulletPos.y >= 0.0f && bulletPos.y <= 3.0f) {
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

	if (isBossSpawned && finalBoss.active)
        {
            float dist = Vector3Distance(bullets[i].position, finalBoss.position);
            
            if (dist < 1.1f) 
            {
                finalBoss.health -= 100.0f;
                
                Vector3 pushDir = Vector3Normalize(bullets[i].velocity);
                ApplyBossKnockback(&finalBoss, Vector3Scale(pushDir, 5.0f)); 
                
                if (finalBoss.health <= 0.0f)
                {
                    finalBoss.active = false;
                    printf("🎉 보스 처치 완료! 🎉\n");
                }

                bullets[i].active = false;
                continue; 
            }
        }
        bullets[i].position = Vector3Add(
            bullets[i].position,
            Vector3Scale(bullets[i].velocity, deltaTime)
        );
	bool hitSomething = false;
        Vector3 pushDirection = Vector3Normalize(bullets[i].velocity); 
        float knockbackPower = 15.0f;

	for (int j = 0; j < 1000; j++) {
            if (!enemies[j].active) continue; // 활성화된 적만 체크

            if (CheckBulletHitEntity(bullets[i].position, enemies[j].position, 0.6f)) {
                printf("Enemy [%d] hit!\n", j);
                enemies[j].health -= 34.0f;
                enemies[j].hitFlashTimer = 0.15f;
                enemies[j].knockback = Vector3Scale(pushDirection, knockbackPower);

                bullets[i].active = false; 
                hitSomething = true;
                break; // 이 총알은 처리 끝났으므로 적 체크 루프 탈출
            }
        }

        // 💡 3. 근접 적(CloseEnemy) 충돌 체크 루프 (일반 적에게 안 맞았을 때만 수행)
        if (!hitSomething) {
            for (int j = 0; j < 1000; j++) {
                if (!closeEnemy[j].active) continue; // 활성화된 근접 적만 체크

                // 근접 적은 피격 반경이 다를 수 있으니 마지막 인자(반지름)를 원하는 대로 조절해 (예: 0.5f 등)
                if (CheckBulletHitEntity(bullets[i].position, closeEnemy[j].position, 0.6f)) {
                    printf("Close Enemy [%d] hit!\n", j);
                    closeEnemy[j].health -= 34.0f;
                    closeEnemy[j].hitFlashTimer = 0.15f;
                    closeEnemy[j].knockback = Vector3Scale(pushDirection, knockbackPower);

                    bullets[i].active = false;
                    hitSomething = true;
                    break;
                }
            }
	}
	if (hitSomething)
        {
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
