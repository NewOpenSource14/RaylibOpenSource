#include "header/close_enemy.h"
#include "header/gameLogic.h"
#include "raylib.h"
#include "raymath.h"

#include "../header/gameLoop.h"
#include "../header/player.h"
#include "../header/userInterface.h"
#include "../header/shootingLogic.h"
#include "../header/map.h"
#include "../header/enemy.h"
#include "../header/fireball.h"
#include "../header/gameLogic.h"
#include "../header/enemyManager.h"
#include "../header/boss.h"

#include <math.h>
#include <stdbool.h>

int screenWidth = 1600;
int screenHeight = 900;

static Camera camera = { 0 };

static Vector2 sensitivity = { 0.001f, 0.001f };
static Vector2 lookRotation = { 0 };

static float headTimer = 0.0f;
static float walkLerp = 0.0f;
static float headLerp = STAND_HEIGHT;
static Vector2 lean = { 0 };

float delta = 0;

void EnemyPlayerSpawnPoint(void);
void spawnBullet(Camera3D cam);

static void UpdateGameLoopBody(
    Body *body,
    float rot,
    char side,
    char forward,
    bool jumpPressed,
    bool crouchHold
);

static void UpdateGameLoopCameraFPS(Camera *camera);
static bool CheckEnemyBodyCollision(Vector3 testPos, float radius);

enum GameScreen currentScreen = GAMEPLAY;

void InitGameLoop(void)
{
    InitWindow(screenWidth, screenHeight, "raylib - Wolfenstein Style Map Design");

    InitEnemyManager();
    InitShooting();
    InitFireballs();

    player.position = GetPlayerStartPosition();
    player.velocity = (Vector3){ 0.0f, 0.0f, 0.0f };
    player.dir = (Vector3){ 0.0f, 0.0f, 0.0f };
    player.isGrounded = true;
    player.health = 100.0f;

    EnemyPlayerSpawnPoint();

    if (!enemy.active)
    {
        InitEnemy(
            &enemy,
            (Vector3){
                player.position.x + 5.0f,
                1.5f,
                player.position.z + 5.0f
            }
        );
    }

    camera.position = (Vector3){
        player.position.x,
        player.position.y + BOTTOM_HEIGHT + STAND_HEIGHT,
        player.position.z
    };

    camera.target = (Vector3){
        player.position.x,
        player.position.y + BOTTOM_HEIGHT + STAND_HEIGHT,
        player.position.z - 1.0f
    };

    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
    SetTargetFPS(60);
}

void UpdateGameLoop(void)
{
    gameLogic(&currentScreen);
    delta = GetFrameTime();
    EnemyManager();

    Vector2 mouseDelta = GetMouseDelta();

    lookRotation.x -= mouseDelta.x * sensitivity.x;
    lookRotation.y += mouseDelta.y * sensitivity.y;

    char sideway = (char)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    char forward = (char)(IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    bool crouching = IsKeyDown(KEY_LEFT_CONTROL);

    UpdateGameLoopBody(
        &player,
        lookRotation.x,
        sideway,
        forward,
        IsKeyPressed(KEY_SPACE),
        crouching
    );

    headLerp = Lerp(
        headLerp,
        (crouching ? CROUCH_HEIGHT : STAND_HEIGHT),
        20.0f * delta
    );

    camera.position = (Vector3){
        player.position.x,
        player.position.y + (BOTTOM_HEIGHT + headLerp),
        player.position.z
    };

    if (player.isGrounded && ((forward != 0) || (sideway != 0)))
    {
        headTimer += delta * 3.0f;
        walkLerp = Lerp(walkLerp, 1.0f, 10.0f * delta);
        camera.fovy = Lerp(camera.fovy, 55.0f, 5.0f * delta);
    }
    else
    {
        walkLerp = Lerp(walkLerp, 0.0f, 10.0f * delta);
        camera.fovy = Lerp(camera.fovy, 60.0f, 5.0f * delta);
    }

    lean.x = Lerp(lean.x, sideway * 0.02f, 10.0f * delta);
    lean.y = Lerp(lean.y, forward * 0.015f, 10.0f * delta);

    UpdateGameLoopCameraFPS(&camera);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && total_bullets > 0)
    {
        spawnBullet(camera);
    }

    UpdateFireballs(delta, &player);
}

void DrawGameLoop(void)
{
    ClearBackground(RAYWHITE);
    BeginDrawing();
	switch(currentScreen)
	{
	    case GAMEPLAY:
	    {
		BeginMode3D(camera);
		MapRender();
		for (int i = 0; i < 1000; i++)
		{
		    if (enemies[i].active)
		    {
			DrawEnemy(&enemies[i]);
		    }
		}
		for (int i = 0; i < 1000; i++)
		{
		    if (closeEnemy[i].active)
		    {
			DrawCloseEnemy(&closeEnemy[i]);
		    }
		}
		if (isBossSpawned && finalBoss.active)
		{
		    DrawBoss(&finalBoss);
		}
		DrawFireballs();
		UpdateAndDrawBullets(&enemy);
		EndMode3D();
		Interface();
		DrawFPS(10, 10);
	    } break;
	    case ENDING:
	    {
		DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
		DrawText("You are Dead", 20, 20, 40, PINK);
		DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, RED);

	    } break;
	    case VICTORY:
	    {
		// 이겼으니까 화사하게 노란색/금색 계열로 장식!
		DrawRectangle(0, 0, screenWidth, screenHeight, GOLD); 
		
		DrawText(" BOSS DEFEATED! ", 20, 20, 40, MAROON);
		DrawText("CONGRATULATIONS! YOU WIN THE GAME!", 20, 80, 25, BLACK);
		
		DrawText("PRESS ENTER or TAP to PLAY AGAIN", 120, 220, 20, DARKGREEN);
	    } break;
	    default: break;
	}
    EndDrawing();
}

void CloseGameLoop(void)
{
    CloseWindow();
}

static void UpdateGameLoopBody(
    Body *body,
    float rot,
    char side,
    char forward,
    bool jumpPressed,
    bool crouchHold
)
{
    Vector2 input = (Vector2){ (float)side, (float)-forward };

    if ((side != 0) && (forward != 0))
    {
        input = Vector2Normalize(input);
    }

    float delta = GetFrameTime();

    if (!body->isGrounded)
    {
        body->velocity.y -= GRAVITY * delta;
    }

    if (body->isGrounded && jumpPressed)
    {
        body->velocity.y = JUMP_FORCE;
        body->isGrounded = false;
    }

    Vector3 front = (Vector3){
        sinf(rot),
        0.0f,
        cosf(rot)
    };

    Vector3 right = (Vector3){
        cosf(-rot),
        0.0f,
        sinf(-rot)
    };

    Vector3 desiredDir = (Vector3){
        input.x * right.x + input.y * front.x,
        0.0f,
        input.x * right.z + input.y * front.z
    };

    body->dir = Vector3Lerp(body->dir, desiredDir, CONTROL * delta);

    float decel = (body->isGrounded ? FRICTION : AIR_DRAG);

    Vector3 hvel = (Vector3){
        body->velocity.x * decel,
        0.0f,
        body->velocity.z * decel
    };

    float hvelLength = Vector3Length(hvel);

    if (hvelLength < (MAX_SPEED * 0.01f))
    {
        hvel = (Vector3){ 0.0f, 0.0f, 0.0f };
    }

    float speed = Vector3DotProduct(hvel, body->dir);

    float maxSpeed = (crouchHold ? CROUCH_SPEED : MAX_SPEED);

    float accel = Clamp(maxSpeed - speed, 0.0f, MAX_ACCEL * delta);

    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    float playerRadius = 0.2f;

    Vector3 nextPosX = body->position;
    nextPosX.x += body->velocity.x * delta;

    if (!CheckMapCollision(nextPosX, playerRadius) &&
        !CheckEnemyBodyCollision(nextPosX, playerRadius))
    {
        body->position.x = nextPosX.x;
    }
    else
    {
        body->velocity.x = 0.0f;
    }

    Vector3 nextPosZ = body->position;
    nextPosZ.z += body->velocity.z * delta;

    if (!CheckMapCollision(nextPosZ, playerRadius) &&
        !CheckEnemyBodyCollision(nextPosZ, playerRadius))
    {
        body->position.z = nextPosZ.z;
    }
    else
    {
        body->velocity.z = 0.0f;
    }

    body->position.y += body->velocity.y * delta;

    float floorHeight = GetMapFloorHeight(body->position, 0.35f);

    if (body->position.y <= floorHeight && body->velocity.y <= 0.0f)
    {
        body->position.y = floorHeight;
        body->velocity.y = 0.0f;
        body->isGrounded = true;
    }
    else
    {
        body->isGrounded = false;
    }
}

static bool CheckEnemyBodyCollision(Vector3 testPos, float radius)
{
    if (!enemy.active)
    {
        return false;
    }

    Vector2 playerPos2D = {
        testPos.x,
        testPos.z
    };

    Vector2 enemyPos2D = {
        enemy.position.x,
        enemy.position.z
    };

    float enemyRadius = 0.65f;

    return CheckCollisionCircles(
        playerPos2D,
        radius,
        enemyPos2D,
        enemyRadius
    );
}

static void UpdateGameLoopCameraFPS(Camera *camera)
{
    const Vector3 up = (Vector3){ 0.0f, 1.0f, 0.0f };
    const Vector3 targetOffset = (Vector3){ 0.0f, 0.0f, -1.0f };

    Vector3 horizontalForward = Vector3RotateByAxisAngle(
        targetOffset,
        up,
        lookRotation.x
    );

    float maxAngleUp = Vector3Angle(up, horizontalForward);
    maxAngleUp -= 0.001f;

    if (-(lookRotation.y) > maxAngleUp)
    {
        lookRotation.y = -maxAngleUp;
    }

    float maxAngleDown = Vector3Angle(Vector3Negate(up), horizontalForward);
    maxAngleDown *= -1.0f;
    maxAngleDown += 0.001f;

    if (-(lookRotation.y) < maxAngleDown)
    {
        lookRotation.y = -maxAngleDown;
    }

    Vector3 right = Vector3Normalize(
        Vector3CrossProduct(horizontalForward, up)
    );

    float pitchAngle = -lookRotation.y - lean.y;

    pitchAngle = Clamp(
        pitchAngle,
        -PI / 2 + 0.0001f,
        PI / 2 - 0.0001f
    );

    Vector3 pitch = Vector3RotateByAxisAngle(
        horizontalForward,
        right,
        pitchAngle
    );

    float headSin = sinf(headTimer * PI);
    float headCos = cosf(headTimer * PI);

    const float stepRotation = 0.01f;

    camera->up = Vector3RotateByAxisAngle(
        up,
        pitch,
        headSin * stepRotation + lean.x
    );

    const float bobSide = 0.1f;
    const float bobUp = 0.15f;

    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    camera->position = Vector3Add(
        camera->position,
        Vector3Scale(bobbing, walkLerp)
    );

    camera->target = Vector3Add(camera->position, pitch);
}
