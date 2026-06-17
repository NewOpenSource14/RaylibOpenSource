#ifndef CLOSE_ENEMY_H
#define CLOSE_ENEMY_H

#include "raylib.h"

typedef struct CloseEnemy
{
    float health;
    Vector3 position;
    float speed;
    float detectRange;
    
    // 💡 근거리 적 전용 변수
    float attackRange;       // 공격 가능 범위 (예: 1.5)
    float attackCooldown;    // 공격 간격 (초 단위, 예: 1.5초)
    float attackTimer;       // 쿨다운 계산용 타이머
    float attackAnimTimer;   // 무기 휘두르는 애니메이션 타이머

    bool isDashing;          // 현재 돌진 중인가?
    float dashTimer;         // 돌진 남은 시간
    float dashCooldownTimer; // 다음 돌진까지 남은 쿨타임

    bool active;
    Vector3 knockback;
    float hitFlashTimer;

} CloseEnemy;

// 근거리 적 초기화
void InitCloseEnemy(CloseEnemy* enemy, Vector3 startPos);

// 근거리 적 업데이트 (플레이어 체력 포인터를 받아 직접 데미지를 입힙니다)
void UpdateCloseEnemy(CloseEnemy* enemy, Vector3 playerPos, float* playerHealth, float deltaTime);

// 근거리 적 그리기
void DrawCloseEnemy(CloseEnemy* enemy);

#endif