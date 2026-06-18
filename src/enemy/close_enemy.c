#include "../../header/close_enemy.h"
#include "../../header/main.h"
#include "../../header/player.h"
#include "../../header/enemyManager.h"

#include "header/map.h"
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>

#define CLOSE_ENEMY_RADIUS 0.3f

// main.c 에 있는 맵 데이터 공유 사용
extern int myNewMap[MAP_HEIGHT][MAP_WIDTH];
extern Vector3 mapPosition;

// -----------------------------------------------------------------------------
// 벽 충돌 검사
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// 근거리 적 초기화
// -----------------------------------------------------------------------------
void InitCloseEnemy(CloseEnemy* enemy, Vector3 startPos)
{
    enemy->health = 120; 
    enemy->position = startPos;
    enemy->speed = 2.5f; 
    enemy->detectRange = 12.0f * WORLD_SCALE;
    
    enemy->attackRange = 1.5f * WORLD_SCALE; 
    enemy->attackCooldown = 1.2f; 
    enemy->attackTimer = 0.0f;
    enemy->attackAnimTimer = 0.0f;

    enemy->active = true;
    enemy->knockback = (Vector3){ 0.0f, 0.0f, 0.0f };
    enemy->hitFlashTimer = 0.0f;

    // 💡 [추가] 돌진 관련 변수 초기화
    enemy->isDashing = false;
    enemy->dashTimer = 0.0f;
    enemy->dashCooldownTimer = 0.0f;
}

// -----------------------------------------------------------------------------
// 근거리 적 업데이트
// -----------------------------------------------------------------------------
void UpdateCloseEnemy(CloseEnemy* enemy, Vector3 playerPos, float* playerHealth, float deltaTime)
{
    if (enemy->health <= 0)
    {
        enemy->active = false;
        enemy->hitFlashTimer = 0.0f;
        enemy->attackAnimTimer = 0.0f;
	totalKilledEnemies += 1;
        enemy->isDashing = false;
    }

    if (!enemy->active) return;

    // 타이머 일괄 업데이트
    if (enemy->hitFlashTimer > 0.0f) enemy->hitFlashTimer -= deltaTime;
    if (enemy->attackAnimTimer > 0.0f) enemy->attackAnimTimer -= deltaTime;
    if (enemy->attackTimer > 0.0f) enemy->attackTimer -= deltaTime;
    
    // 💡 [추가] 돌진 타이머 및 쿨타임 업데이트
    if (enemy->dashCooldownTimer > 0.0f) enemy->dashCooldownTimer -= deltaTime;
    if (enemy->dashTimer > 0.0f) 
    {
        enemy->dashTimer -= deltaTime;
    }
    else 
    {
        enemy->isDashing = false; // 돌진 시간이 끝나면 일반 상태로 복귀
    }

    // 넛백 처리
    if (Vector3Length(enemy->knockback) > 0.05f)
    {
        Vector3 nextPosX = enemy->position;
        nextPosX.x += enemy->knockback.x * deltaTime;
        if (!CheckMapCollision(nextPosX, CLOSE_ENEMY_RADIUS)) enemy->position.x = nextPosX.x;

        Vector3 nextPosZ = enemy->position;
        nextPosZ.z += enemy->knockback.z * deltaTime;
        if (!CheckMapCollision(nextPosZ, CLOSE_ENEMY_RADIUS)) enemy->position.z = nextPosZ.z;

        float friction = 1.0f - (10.0f * deltaTime);
        if (friction < 0.0f) friction = 0.0f;
        enemy->knockback = Vector3Scale(enemy->knockback, friction);
    }

    // 플레이어와의 방향 및 거리 계산
    Vector3 direction = Vector3Subtract(playerPos, enemy->position);
    direction.y = 0.0f; 
    float distance = Vector3Length(direction);

    // 플레이어 감지 범위 내에 있을 때
    if (distance < enemy->detectRange)
    {
        // 💡 [핵심 추가] 돌진 조건 체크: 돌진 중이 아니고, 돌진 쿨타임이 끝났다면 순간 돌진 트리거!
        if (!enemy->isDashing && enemy->dashCooldownTimer <= 0.0f)
        {
            enemy->isDashing = true;
            enemy->dashTimer = 0.5f;          // 0.5초 동안 순간적으로 질주
            enemy->dashCooldownTimer = 2.5f;  // 한 번 돌진하면 4초 동안 재돌진 불가
            printf("⚠️ 근거리 적이 플레이어를 조준하고 폭발적으로 돌진합니다!\n");
        }

        // 공격 사정거리 "밖"일 때 플레이어를 향해 전진
        if (distance > enemy->attackRange)
        {
            direction = Vector3Normalize(direction);

            // 💡 [핵심 추가] 현재 돌진 중이라면 속도를 2.5배 뻥튀기합니다.
            float currentSpeed = enemy->speed;
            if (enemy->isDashing)
            {
                currentSpeed *= 3.5f; // 기본 2.5 -> 돌진 시 6.25의 속도로 좁혀옴
            }

            // X축 이동
            Vector3 nextPosX = enemy->position;
            nextPosX.x += direction.x * currentSpeed * deltaTime;
            if (!CheckMapCollision(nextPosX, CLOSE_ENEMY_RADIUS)) enemy->position.x = nextPosX.x;

            // Z축 이동
            Vector3 nextPosZ = enemy->position;
            nextPosZ.z += direction.z * currentSpeed * deltaTime;
            if (!CheckMapCollision(nextPosZ, CLOSE_ENEMY_RADIUS)) enemy->position.z = nextPosZ.z;
        }
        // 공격 사정거리 "안"에 들어왔고, 공격 쿨다운이 끝났을 때
        else if (enemy->attackTimer <= 0.0f)
        {
            enemy->isDashing = false; // 공격 본능이 발동하면 돌진 강제 해제
            enemy->dashTimer = 0.0f;

            enemy->attackTimer = enemy->attackCooldown; 
            enemy->attackAnimTimer = 0.25f;            
            
            if (playerHealth != NULL)
            {
                *playerHealth -= 20.0f;
                printf("플레이어가 근거리 공격을 당했습니다! 남은 체력: %.1f\n", *playerHealth);
            }
        }
    }
    else
    {
        // 플레이어를 시야에서 완전히 놓쳤다면, 다음에 만났을 때 바로 돌진할 수 있게 쿨타임을 살짝 경감
        if (enemy->dashCooldownTimer > 1.0f) enemy->dashCooldownTimer = 1.0f;
    }
}

// -----------------------------------------------------------------------------
// 근거리 적 렌더링
// -----------------------------------------------------------------------------
void DrawCloseEnemy(CloseEnemy* enemy)
{
    if (!enemy->active) return;

    Vector3 drawPos = enemy->position;
    drawPos.y = 0.75f; // 몸통 중심 높이

    // 상태에 따른 색상 정의
    bool isHit = (enemy->hitFlashTimer > 0.0f);
    Color bodyColor = isHit ? RED : ORANGE;
    Color wireColor = isHit ? MAROON : BROWN; 
    Color headColor = isHit ? PINK : YELLOW;

    // 돌진 중 시각 효과 (황금색)
    if (!isHit && enemy->isDashing) 
    {
        bodyColor = GOLD;
        wireColor = ORANGE;
    }

    // [1] 적 몬스터 본체 그리기 (몸통 + 머리)
    DrawCube(drawPos, 1.0f, 1.5f, 1.0f, bodyColor);
    DrawCubeWires(drawPos, 1.0f, 1.5f, 1.0f, wireColor);
    DrawSphere((Vector3){ drawPos.x, drawPos.y + 1.1f, drawPos.z }, 0.25f, headColor);


    // [2] 💡 [핵심 수정] 무기 휘두르기 (옆으로 부채꼴 궤적) 렌더링
    if (enemy->attackAnimTimer > 0.0f)
    {
        // 2-1. 적의 위치에서 플레이어 위치를 바라보는 방향 벡터 계산
        Vector3 dirToPlayer = Vector3Subtract(player.position, enemy->position);
        dirToPlayer.y = 0.0f; // 평면상에서 계산하기 위해 Y축 고정
        
        if (Vector3Length(dirToPlayer) > 0.001f) {
            dirToPlayer = Vector3Normalize(dirToPlayer);
        } else {
            dirToPlayer = (Vector3){ 0.0f, 0.0f, 1.0f }; // 예외 상황용 기본 정면
        }

        // 2-2. 💡 [휘두르기 수학] 수직 벡터(Normal) 구하기
        // 현재 평면(XZ평면)에서 방향 벡터(`dirToPlayer`)와 수직인 벡터(예: Y축 단위벡터)를 사용해,
        // 양옆(부채꼴)을 가리키는 수직 법선 벡터(`swingNormal`)를 계산합니다.
        Vector3 upNormal = { 0.0f, 1.0f, 0.0f };
        Vector3 swingNormal = Vector3CrossProduct(dirToPlayer, upNormal); // 옆을 가리키는 벡터 도출

        // 2-3. 공격 애니메이션 진행도에 따른 각도 계산
        // 진행도 (0.0 ~ 1.0): 0.25초 타이머가 줄어드는 것을 역산하여 진행도를 구합니다.
        float swingProgress = (0.25f - enemy->attackAnimTimer) * 4.0f; // 0.0에서 1.0으로 증가

        // 휘두르는 범위 (부채꼴 각도) 설정: 양옆으로 -60도에서 +60도까지 (총 120도 범위)
        float swingRange = 60.0f * DEG2RAD; // 60도를 라디안으로 변환
        
        // 진행도(`swingProgress`)에 따라 각도를 -60도에서 +60도로 선형 보간(Lerp)하여 실제 휘두르는 각도를 구합니다.
        float currentSwingAngle = (swingProgress * 2.0f - 1.0f) * swingRange; // -60도 ~ +60도


        // 2-4. 💡 [무기 끝점 계산] 방향 벡터에 법선 벡터를 비례하여 더해 부채꼴 궤적을 만듭니다.
        // 정면 방향 벡터(`dirToPlayer`)에 계산한 휘두르는 각도의 수직 법선 벡터(`swingNormal`)를 더해주면,
        // 무기가 정면에서 양옆으로 휘어지며 부채꼴 형태의 끝점(EndPos)이 나옵니다.
        Vector3 currentWeaponDirection = Vector3Add(dirToPlayer, Vector3Scale(swingNormal, currentSwingAngle));
        currentWeaponDirection = Vector3Normalize(currentWeaponDirection); // 벡터 정규화로 길이 1 유지

        // 무기(막대기)의 길이 설정
        float stickLength = 1.6f; 
        
        // 적의 몸통 중심높이(Y = 0.75f)에서 무기를 앞으로 내밉니다.
        Vector3 stickStart = drawPos; 
        
        // 계산한 부채꼴 방향벡터(`currentWeaponDirection`)로 길이만큼 전진한 끝점 좌표 도출
        Vector3 stickEnd = Vector3Add(stickStart, Vector3Scale(currentWeaponDirection, stickLength));


        // 2-5. [무기 그리기] DrawCylinderEx로 휘두르는 궤적상의 막대기 렌더링
        float stickRadius = 0.08f; // 막대기 두께
        
        // 💡 휘두르는 궤적 시각 효과를 위해 공격 색상을 RED로 설정
        // 1. 약간 더 두꺼운 테두리용 실린더를 먼저 어두운 색(wireColor)으로 그립니다.
        float wireRadius = stickRadius + 0.02f; // 테두리 두께 확보 (+0.02f)
        DrawCylinderEx(stickStart, stickEnd, wireRadius, wireRadius, 8, wireColor);
        
        // 2. 그 위에 원래 두께의 알맹이 무기(RED)를 덮어씌워 렌더링합니다.
        DrawCylinderEx(stickStart, stickEnd, stickRadius, stickRadius, 8, RED);
    }
}
