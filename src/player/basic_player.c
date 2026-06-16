#include "raylib.h"
#include "raymath.h"

#include "../../header/player.h"
#include "../../header/main.h"
#include "../../header/map.h"
#include "../../header/camera.h"
Body player = { 0 };

//얼마나 마우스를 빨리 움직일 것인지
static Vector2 sensitivity = { 0.001f, 0.001f };

void PlayerMouseMovement(){
	// 마우스가 지난 프레임보다 얼마나 더 움직였니?
        Vector2 mouseDelta = GetMouseDelta();
	// 이것은 카메라 로테이션을 다룬다 디테일 한것은 노트에 있다.(1)
	// 3D그래픽스는 표준 수학은 반시계 방향이 왼쪽이 +다
	// 그래서 오른쪽으로 돌리려면 (+)값 오른쪽 마우스 방향을 빼줘야 오른쪽으로 돌아간다
        lookRotation.x -= mouseDelta.x * sensitivity.x;
        lookRotation.y += mouseDelta.y * sensitivity.y;
}



void UpdateBody(Body *body, float rot, bool jumpPressed)
{
    //이거는 옆앞 WDSA를 설정하는것이다
    char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    bool crouching = IsKeyDown(KEY_LEFT_CONTROL);

    //z축은 -가 붙는 이유는 앞으로 가는게 - 그리고 뒤로 가는게 +다
    Vector2 input = (Vector2){ (float)sideway, (float)-forward };
    //normalize즉 백터의 방향을 정하는거다 --> 백터를 다 0~1로 맞추는 과정이다
    if ((sideway != 0) && (forward != 0)) input = Vector2Normalize(input);

    //Frame단위 시간
    float delta = GetFrameTime();

    //body가 만약 바닥에 있지 않는다면 GRAVITY를 적용한다
    if (!body->isGrounded) body->velocity.y -= GRAVITY * delta;
    
    //만약 점프를 눌렀다면??
    if (body->isGrounded && jumpPressed)
    {
	//그거를 실제로 적용하는 과정
	//솔직히 너 이정도는 알아야 해
        body->velocity.y = JUMP_FORCE;
        body->isGrounded = false;
    }


    //여기는 방향을 정하는거임
    //front는 앞 direction이다
    Vector3 front = (Vector3){ sinf(rot), 0.f, cosf(rot) };
    //right는 옆 direction이다
    Vector3 right = (Vector3){ cosf(-rot), 0.f, sinf(-rot) };
    //이거는 모든것을 고려하는것이다
    //이것이 진짜 플레이어의 다이렉션이다
    Vector3 desiredDir = (Vector3){ input.x * right.x + input.y * front.x, 0.0f, input.x * right.z + input.y * front.z, };
    
    //기존 body direction에서 desiredDir(우리가 적은것 방금)을 부드럽게 넘어가겠금 하는 변수이다
    body->dir = Vector3Lerp(body->dir, desiredDir, CONTROL * delta);

    //만약에 바닥에 붙어있으면 FRICTION을 붙침 아니면 AIR_DRAG를 적용
    float decel = (body->isGrounded ? FRICTION : AIR_DRAG);
    //FRICTION을 이제 곱해주는거임 velocity값에
    Vector3 hvel = (Vector3){ body->velocity.x * decel, 0.0f, body->velocity.z * decel };

    //그리고 그 velocity의 기리를 구하는거임
    float hvelLength = Vector3Length(hvel);
    //만약 velocity*FRICTION의 길이가 MAX_SPEED를 넘어가면 안됌
    if (hvelLength < (MAX_SPEED * 0.01f)) hvel = (Vector3){ 0 };

    //내적을 구하면 스피드가 나옴
    //이거는 노트에 더 디테일하게 나와있다
    float speed = Vector3DotProduct(hvel, body->dir);

    //MAX_SPEED는 얼마인지를 본다
    float maxSpeed = (crouching ? CROUCH_SPEED : MAX_SPEED);
    //이거는 가속도인데 만약 speed가 80이고 maxSpeed가 100이면 20임 그러면 0 과 20 으로 Clamp를 하는것이다
    float accel = Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * delta);
    //그리고 가속도를 붙치는것이다 실제 dir에다가 곱하는것이다
    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    //이제 모든것을 
    //veclocity.x = FRICTION * dir * accel 까지 진짜 speed가 만들어지는 과정이다
    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    float playerRadius = 0.2f;

    //이동을 시도하는 과정
    //현 dir그리고 스피드(velocity)를 넘겨준다
    Vector3 nextPosX = body->position;
    nextPosX.x += body->velocity.x * delta;
    
    if (!CheckMapCollision(nextPosX, playerRadius)) {
        body->position.x = nextPosX.x; // 벽이 없으면 X축 전진
    } else {
        body->velocity.x = 0.0f;       // 막혔다면 X축 속도만 죽임
    }

    // 2. Z축 이동 시도
    Vector3 nextPosZ = body->position;
    nextPosZ.z += body->velocity.z * delta;

    if (!CheckMapCollision(nextPosZ, playerRadius)) {
        body->position.z = nextPosZ.z; // 벽이 없으면 Z축 전진
    } else {
        body->velocity.z = 0.0f;       // 막혔다면 Z축 속도만 죽임
    }

    // 3. Y축(점프/중력) 처리
    body->position.y += body->velocity.y * delta;
    //그냥 바닥에 있거나 바닥을 뚫고 밑으로 들어가려고 하면????
    if (body->position.y <= 0.0f)
    {
        body->position.y = 0.0f;
        body->velocity.y = 0.0f;
        body->isGrounded = true; 
    }
    headLerp = Lerp(headLerp, (crouching ? CROUCH_HEIGHT : STAND_HEIGHT), 20.0f * delta);
    camera.position = (Vector3){
	player.position.x,
	//이것은 BOTTOM_HEIGHT = 0.5f 그리고 headlerp = crtl를 누르면 0 그리고 아니면 1.0f를 준다
	player.position.y + (BOTTOM_HEIGHT + headLerp),
	player.position.z,
    };
    //이코드는 카메라 시야 좁히는 코드
    //만약 player가 땅에 있고 움직이고 있다면
    if (player.isGrounded && ((forward != 0) || (sideway != 0)))
    {
	//사람이 거를떄마다 타이머다 이것은 오로지 캐릭터가 걸을때만 작용되는 것입니다
	headTimer += delta * 3.0f;
	//화면이 출렁거리는것을 표현 0.0부터 1.0 까지 부드럽게 표현하는 선형 보간법
	walkLerp = Lerp(walkLerp, 1.0f, 10.0f * delta);
	//카메라 fovy즉 카메라를 돌리는거다 60.0f에서 55.0f으로 축소를 하는것이다
	camera.fovy = Lerp(camera.fovy, 55.0f, 5.0f * delta); // 시야 좁아짐 연출
    }
    else
    {
	//다시 원상복구 하는것이다
	walkLerp = Lerp(walkLerp, 0.0f, 10.0f * delta);
	camera.fovy = Lerp(camera.fovy, 60.0f, 5.0f * delta);
    }
    //여기는 실제 연출 즉 움직이는것을 연출
    //lean은 일종에 백터인데 Vector2(0)
    //좌우로 0.02f만큼 기우는걸 연출하기 위한 과정
    //이제는 너도 lerp이 뭔지 아니깐 걍 읽으삼
    lean.x = Lerp(lean.x, sideway * 0.02f, 10.0f * delta); // 좌우 몸 기울임 연출
    lean.y = Lerp(lean.y, forward * 0.015f, 10.0f * delta);
}
