#include "raylib.h"
#include "raymath.h"

#include "../../header/camera.h"
#include "../../header/main.h"
#include "../../header/player.h"
Camera camera = { 0 };

float headTimer = 0.0f;
float walkLerp = 0.0f;
float headLerp = STAND_HEIGHT;
Vector2 lean = { 0 };
Vector2 lookRotation = { 0 };

void UpdateCameraFPS(Camera *camera)
{
    //위를 바라보는 백터
    const Vector3 up = (Vector3){ 0.0f, 1.0f, 0.0f };
    //앞을 바라보는 백터
    const Vector3 targetOffset = (Vector3){ 0.0f, 0.0f, -1.0f };
    // 앞쪽 화살표를 Y축(up) 기준으로 좌우(lookRotation.x)로만 돌려놓은 내 몸통의 정면 방향--> 그냥 내가 현제 발아보고 있는 방향이다
    // 고개를 도리도리 하게 하는것
    Vector3 horizontalForward = Vector3RotateByAxisAngle(targetOffset, up, lookRotation.x);

    //하지만 이거는 어디까지나 그냥 90도 수직이다 up은 어디까지나 90도고
    //horizontalForward는 어디까지나 z축 방향이다
    
    //우리는 현제 도리도리를 보는거기는 하지만 혹시 어디까지 끄덕끄덕을 할 수 있어
    float maxAngleUp = Vector3Angle(up, horizontalForward);
    //약간 줄이는거임 만약 80도라고 가정을 하면 79.999도로 줄이는 거임
    maxAngleUp -= 0.001f;
    //이거는 90도를 못 넘어가게 잡아주는 것이다
    if ( -(lookRotation.y) > maxAngleUp) { lookRotation.y = -maxAngleUp; }

    //이거는 -90도다 근데 어짜피 다시 90도로 바꿀거긴 하다
    //밑에하고 완전히 똑같은 코드이지만 밑을 -90도까지 돌리지 못하겠금 한다.
    float maxAngleDown = Vector3Angle(Vector3Negate(up), horizontalForward);
    maxAngleDown *= -1.0f; 
    maxAngleDown += 0.001f; 
    if ( -(lookRotation.y) < maxAngleDown) { lookRotation.y = -maxAngleDown; }

    //이제 외적을 구하는건데... 이거는 그냥 노트에 더 디테일하게 나와있음
    //새로운 백터를 구하는 것이다 horizontalForward와 up 기준으로
    //우리가 maxAngleDown/maxAngleUp을 89.999로 설정한 이유가 여기서 나온다
    Vector3 right = Vector3Normalize(Vector3CrossProduct(horizontalForward, up));

    //player이 실제로 움직임 즉 head bobbing도 고려
    float pitchAngle = -lookRotation.y - lean.y;
    //이제 실제 끄덕끄던 적용하는 공간임
    pitchAngle = Clamp(pitchAngle, -PI/2 + 0.0001f, PI/2 - 0.0001f); 
    //z는 horizontalForward 그리고 x는 right 그리고 pitchAngle만큼 위아래로 회전을 시켜라
    //이것이 최종 형태다 끄덕끄덕 + 도리로리
    Vector3 pitch = Vector3RotateByAxisAngle(horizontalForward, right, pitchAngle);

    // 카메라 기울임 및 출렁임 연출 (역동감 복구!)
    // sin그리고 cos는 알다시피 파동 물결이다
    // t가 올라갈수록 파동 효과가 일어나거 마치 head가 움직이는 것처럼 보인다
    float headSin = sinf(headTimer * PI);
    float headCos = cosf(headTimer * PI);
    const float stepRotation = 0.01f;
    //이제 모든것을 합하는것이다
    camera->up = Vector3RotateByAxisAngle(up, pitch, headSin * stepRotation + lean.x);

    //좌우 흔들림 최대치
    const float bobSide = 0.1f;
    const float bobUp = 0.15f;
    //이제 모든것을 다 합치는 과정
    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    //fabsf는 그냥 절댓값이다
    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walkLerp));
    camera->target = Vector3Add(camera->position, pitch);
}
