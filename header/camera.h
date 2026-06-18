#pragma once
#include <raylib.h>

//이거는 정확의 뭔지 잘 모름 이따가 돌아오겠음
extern float headTimer;
extern float walkLerp;
extern float headLerp;
extern Vector2 lean;
//rotation 인간이 얼마나 요리보고 조리보고 할 수 있는지
extern Vector2 lookRotation;
extern Camera3D camera;

//함수
void UpdateCameraFPS(Camera *camera);
