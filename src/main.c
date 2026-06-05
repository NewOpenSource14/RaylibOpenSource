#include "raylib.h"
#include "../header/shootingLogic.h"
#include "../header/main.h"
#include "../header/enemy.h"
#include "../header/fireball.h"
#include "../header/camera.h"
#include "../header/map.h"
#include "../header/userInterface.h"

//map이 정확의 어디에 위치를 할건지에 대한 내용
Vector3 mapPosition = { -8.0f, 0.0f, -8.0f };  

//이거는 그냥 screen을 소환하는거임
const int screenWidth = 800;
const int screenHeight = 450;

float shutterOpenTimer = 0.0f;
float shutterHoldTimer = 0.0f;
//전역 변수들

//이거는 실제 플레이어
Body player = { 0 };
// 적 관련 변수들
Enemy enemy;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "raylib - Smooth Wall Sliding & Head Bobbing");

    //마우스하고 관련은 없지만 너을때가 없음
    player.health = 100;

    //총알 초기화
    InitShooting();
    //적 파이어볼 공격 초기화
    InitFireballs();

    camera.fovy = 60.0f;
    //그것이 Camera Perspective
    camera.projection = CAMERA_PERSPECTIVE;

    //enemy및 player spawn
    EnemeyPlayerSpawnPoint();
    
    //커서는 안보이게 하는것
    DisableCursor(); 
    //FPS는 60프레임으로 마추는 것이다
    SetTargetFPS(60);

    while (!WindowShouldClose())    
    {
	//player마우스 움직임
	PlayerMouseMovement();

        //마우스 클릭을 받는곳
        //총알을 쏘기 위한 로직
        ShootingLogic();

        // UpdateBody 안에서 X/Z축 이동과 스무스한 충돌 처리를 모두 완료합니다.
        UpdateBody(&player, lookRotation.x ,IsKeyPressed(KEY_SPACE));

        // 적 업데이트 관련 함수
        UpdateEnemy(&enemy, player.position, GetFrameTime());

        // 파이어볼 업데이트 관련 함수
        UpdateFireballs(GetFrameTime(), &player);
	
	//이제 카메라 업데이트에다가 (주소camera) 넘겨주는 과정
        UpdateCameraFPS(&camera); 

        BeginDrawing();
	    //전부다 하얀색으로 칠하는거임 배경을
            ClearBackground(RAYWHITE); 
	    //카메라 시작
            BeginMode3D(camera);
                //바로 바닥을 까라주는 코드가
		//Map Width와 Map Height만큼
		// 적 그리기
                DrawEnemy(&enemy);
		// 파이어볼 그리기
                DrawFireballs();
		//map을 실제로 렌더링
		MapRender();
                //이것은 총알을 업데이트하고 그려주는 함수이다
                UpdateAndDrawBullets(&enemy);
            //그릴건 다 끝났으니 해재한다
            EndMode3D();
	    //이거는 내 프로그램의 FPS를 뛰어주는 창이다
            DrawFPS(10, 10);
	    //UI는 모두다 여기서 관리
	    Interface();
	    
        EndDrawing();
    }

    CloseWindow();                  
    return 0;
}
