#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include <math.h>

static Vector3 mapPosition = {
    -((MAP_WIDTH * WORLD_SCALE) / 2.0f),
    0.0f,
    -((MAP_HEIGHT * WORLD_SCALE) / 2.0f)
};

static Body player = { 0 };
static Vector2 lookRotation = { 0 };
static float headTimer = 0.0f;
static float walkLerp = 0.0f;
static float headLerp = STAND_HEIGHT;
static Vector2 lean = { 0 };

static bool isShutterOpen = false;
static float shutterOpenTimer = 0.0f;
static float shutterHoldTimer = 0.0f;

static bool IsSolidTile(int tileType);
static bool IsDoorSideTile(int tileType);
static int GetMapTileSafe(int x, int y);
static bool IsOpenTileForWallFace(int tileType);

static bool CheckMapCollision(Vector3 testPos, float radius);
static void UpdateBody(Body *body, float rot, char side, char forward, bool jumpPressed, bool crouchHold);
static void UpdateCameraFPS(Camera *camera);

static void DrawWolfWall(int x, int y, Vector3 tilePos, Color baseColor, Color panelColor, Color trimColor);
static void DrawDoorTile(int x, int y, Vector3 tilePos);
static void DrawDesk(Vector3 tilePos);
static void DrawTable(Vector3 tilePos);
static void DrawBarrel(Vector3 tilePos);
static void DrawLamp(Vector3 tilePos);
static void DrawMiniMap(void);
static void DrawUI(void);

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Wolfenstein Style Map Design");

    player.position = (Vector3){
        mapPosition.x + (14.5f * WORLD_SCALE),
        0.0f,
        mapPosition.z + (16.5f * WORLD_SCALE)
    };

    Camera camera = { 0 };
    camera.fovy = 85.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        Vector2 mouseDelta = GetMouseDelta();
        float hybridSensitivity = 0.0007f;

        lookRotation.x -= mouseDelta.x * hybridSensitivity;
        lookRotation.y -= mouseDelta.y * hybridSensitivity;
        lookRotation.y = Clamp(lookRotation.y, -89.0f * DEG2RAD, 89.0f * DEG2RAD);

        char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
        char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
        bool crouching = IsKeyDown(KEY_LEFT_CONTROL);

        UpdateBody(&player, lookRotation.x, sideway, forward, IsKeyPressed(KEY_SPACE), crouching);

        headLerp = Lerp(headLerp, (crouching ? CROUCH_HEIGHT : STAND_HEIGHT), 20.0f * delta);

        camera.position = (Vector3){
            player.position.x,
            player.position.y + (BOTTOM_HEIGHT + headLerp),
            player.position.z
        };

        if (player.isGrounded && ((forward != 0) || (sideway != 0)))
        {
            headTimer += delta * 3.0f;
            walkLerp = Lerp(walkLerp, 1.0f, 10.0f * delta);
            camera.fovy = Lerp(camera.fovy, 80.0f, 5.0f * delta);
        }
        else
        {
            walkLerp = Lerp(walkLerp, 0.0f, 10.0f * delta);
            camera.fovy = Lerp(camera.fovy, 85.0f, 5.0f * delta);
        }

        lean.x = Lerp(lean.x, sideway * 0.02f, 10.0f * delta);
        lean.y = Lerp(lean.y, forward * 0.015f, 10.0f * delta);

        UpdateCameraFPS(&camera);

        if (IsKeyPressed(KEY_E))
        {
            isShutterOpen = !isShutterOpen;

            if (isShutterOpen)
            {
                shutterHoldTimer = 0.0f;
            }
        }

        if (isShutterOpen)
        {
            if (shutterOpenTimer < 1.0f)
            {
                shutterOpenTimer += delta * 1.5f;

                if (shutterOpenTimer > 1.0f)
                {
                    shutterOpenTimer = 1.0f;
                }
            }
            else
            {
                shutterHoldTimer += delta;

                if (shutterHoldTimer >= 1.5f)
                {
                    isShutterOpen = false;
                }
            }
        }
        else
        {
            if (shutterOpenTimer > 0.0f)
            {
                shutterOpenTimer -= delta * 1.5f;

                if (shutterOpenTimer < 0.0f)
                {
                    shutterOpenTimer = 0.0f;
                }
            }
        }

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);

                DrawPlane(
                    (Vector3){ 0.0f, 0.0f, 0.0f },
                    (Vector2){ MAP_WIDTH * WORLD_SCALE, MAP_HEIGHT * WORLD_SCALE },
                    (Color){ 42, 42, 42, 255 }
                );

                for (int y = 0; y < MAP_HEIGHT; y++)
                {
                    for (int x = 0; x < MAP_WIDTH; x++)
                    {
                        Vector3 tilePos = {
                            mapPosition.x + x * WORLD_SCALE + (WORLD_SCALE / 2.0f),
                            mapPosition.y,
                            mapPosition.z + y * WORLD_SCALE + (WORLD_SCALE / 2.0f)
                        };

                        switch (myNewMap[y][x])
                        {
                            case TILE_WALL:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 88, 88, 92, 255 },
                                    (Color){ 62, 62, 68, 255 },
                                    BLACK
                                );

                                break;
                            }

                            case TILE_WALL_BLUE:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 7, 18, 78, 255 },
                                    (Color){ 28, 78, 210, 255 },
                                    BLACK
                                );

                                break;
                            }

                            case TILE_WALL_DECOR:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 88, 88, 92, 255 },
                                    (Color){ 145, 42, 35, 255 },
                                    BLACK
                                );

                                break;
                            }

                            case TILE_WALL_FLAG:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 76, 76, 82, 255 },
                                    (Color){ 190, 25, 25, 255 },
                                    BLACK
                                );

                                break;
                            }

                            case TILE_PORTRAIT:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 80, 80, 85, 255 },
                                    (Color){ 150, 105, 50, 255 },
                                    BLACK
                                );

                                break;
                            }

                            case TILE_WALL_DARK:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 28, 28, 32, 255 },
                                    (Color){ 76, 76, 82, 255 },
                                    BLACK
                                );

                                break;
                            }

                            case TILE_COLUMN_CYAN:
                            {
                                tilePos.y = mapPosition.y + (3.5f * WORLD_SCALE / 2.0f);

                                DrawCube(
                                    tilePos,
                                    WORLD_SCALE * 0.62f,
                                    3.5f * WORLD_SCALE,
                                    WORLD_SCALE * 0.62f,
                                    (Color){ 0, 185, 210, 255 }
                                );

                                DrawCubeWires(
                                    tilePos,
                                    WORLD_SCALE * 0.62f,
                                    3.5f * WORLD_SCALE,
                                    WORLD_SCALE * 0.62f,
                                    DARKBLUE
                                );

                                break;
                            }

                            case TILE_PILLAR:
                            {
                                tilePos.y = mapPosition.y + (3.5f * WORLD_SCALE / 2.0f);

                                DrawCube(
                                    tilePos,
                                    WORLD_SCALE * 0.72f,
                                    3.5f * WORLD_SCALE,
                                    WORLD_SCALE * 0.72f,
                                    (Color){ 75, 75, 78, 255 }
                                );

                                DrawCubeWires(
                                    tilePos,
                                    WORLD_SCALE * 0.72f,
                                    3.5f * WORLD_SCALE,
                                    WORLD_SCALE * 0.72f,
                                    BLACK
                                );

                                break;
                            }

                            case TILE_COVER:
                            {
                                tilePos.y = mapPosition.y + (1.2f * WORLD_SCALE / 2.0f);

                                DrawCube(
                                    tilePos,
                                    WORLD_SCALE,
                                    1.2f * WORLD_SCALE,
                                    WORLD_SCALE,
                                    BROWN
                                );

                                DrawCubeWires(
                                    tilePos,
                                    WORLD_SCALE,
                                    1.2f * WORLD_SCALE,
                                    WORLD_SCALE,
                                    DARKBROWN
                                );

                                break;
                            }

                            case TILE_DESK:
                            {
                                DrawDesk(tilePos);
                                break;
                            }

                            case TILE_TABLE:
                            {
                                DrawTable(tilePos);
                                break;
                            }

                            case TILE_BARREL:
                            {
                                DrawBarrel(tilePos);
                                break;
                            }

                            case TILE_LAMP:
                            {
                                DrawLamp(tilePos);
                                break;
                            }

                            case TILE_SHUTTER:
                            {
                                DrawDoorTile(x, y, tilePos);
                                break;
                            }

                            case TILE_EXIT_SIGN:
                            {
                                DrawWolfWall(
                                    x,
                                    y,
                                    tilePos,
                                    (Color){ 30, 30, 34, 255 },
                                    LIME,
                                    GREEN
                                );

                                break;
                            }

                            case TILE_GOAL:
                            {
                                tilePos.y = mapPosition.y + 0.05f;

                                DrawCube(tilePos, WORLD_SCALE, 0.1f, WORLD_SCALE, LIME);
                                DrawCubeWires(tilePos, WORLD_SCALE, 0.1f, WORLD_SCALE, GREEN);

                                break;
                            }
                        }
                    }
                }

            EndMode3D();

            DrawMiniMap();
            DrawUI();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

static bool IsSolidTile(int tileType)
{
    if (tileType == TILE_WALL) return true;
    if (tileType == TILE_WALL_DECOR) return true;
    if (tileType == TILE_WALL_DARK) return true;
    if (tileType == TILE_PILLAR) return true;
    if (tileType == TILE_COVER) return true;
    if (tileType == TILE_EXIT_SIGN) return true;
    if (tileType == TILE_WALL_BLUE) return true;
    if (tileType == TILE_COLUMN_CYAN) return true;
    if (tileType == TILE_DESK) return true;
    if (tileType == TILE_TABLE) return true;
    if (tileType == TILE_WALL_FLAG) return true;
    if (tileType == TILE_BARREL) return true;
    if (tileType == TILE_LAMP) return true;
    if (tileType == TILE_PORTRAIT) return true;
    if (tileType == TILE_SHUTTER && shutterOpenTimer < 0.8f) return true;

    return false;
}

static bool IsDoorSideTile(int tileType)
{
    if (tileType == TILE_WALL) return true;
    if (tileType == TILE_WALL_DECOR) return true;
    if (tileType == TILE_WALL_DARK) return true;
    if (tileType == TILE_WALL_BLUE) return true;
    if (tileType == TILE_EXIT_SIGN) return true;
    if (tileType == TILE_WALL_FLAG) return true;
    if (tileType == TILE_PORTRAIT) return true;

    return false;
}

static int GetMapTileSafe(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return TILE_WALL;
    }

    return myNewMap[y][x];
}

static bool IsOpenTileForWallFace(int tileType)
{
    if (tileType == TILE_EMPTY) return true;
    if (tileType == TILE_GOAL) return true;
    if (tileType == TILE_SHUTTER) return true;
    if (tileType == TILE_DESK) return true;
    if (tileType == TILE_TABLE) return true;
    if (tileType == TILE_COVER) return true;
    if (tileType == TILE_BARREL) return true;
    if (tileType == TILE_LAMP) return true;

    return false;
}

static void DrawWolfWall(int x, int y, Vector3 tilePos, Color baseColor, Color panelColor, Color trimColor)
{
    float wallHeight = 3.5f * WORLD_SCALE;
    float panelHeight = 2.55f * WORLD_SCALE;
    float panelWidth = 0.82f * WORLD_SCALE;
    float panelThickness = 0.06f;

    tilePos.y = mapPosition.y + wallHeight / 2.0f;

    DrawCube(tilePos, WORLD_SCALE, wallHeight, WORLD_SCALE, baseColor);
    DrawCubeWires(tilePos, WORLD_SCALE, wallHeight, WORLD_SCALE, trimColor);

    if (IsOpenTileForWallFace(GetMapTileSafe(x, y - 1)))
    {
        Vector3 panelPos = tilePos;
        panelPos.z -= WORLD_SCALE * 0.505f;
        panelPos.y = mapPosition.y + 2.0f * WORLD_SCALE;

        DrawCube(panelPos, panelWidth, panelHeight, panelThickness, panelColor);
        DrawCubeWires(panelPos, panelWidth, panelHeight, panelThickness, trimColor);
    }

    if (IsOpenTileForWallFace(GetMapTileSafe(x, y + 1)))
    {
        Vector3 panelPos = tilePos;
        panelPos.z += WORLD_SCALE * 0.505f;
        panelPos.y = mapPosition.y + 2.0f * WORLD_SCALE;

        DrawCube(panelPos, panelWidth, panelHeight, panelThickness, panelColor);
        DrawCubeWires(panelPos, panelWidth, panelHeight, panelThickness, trimColor);
    }

    if (IsOpenTileForWallFace(GetMapTileSafe(x - 1, y)))
    {
        Vector3 panelPos = tilePos;
        panelPos.x -= WORLD_SCALE * 0.505f;
        panelPos.y = mapPosition.y + 2.0f * WORLD_SCALE;

        DrawCube(panelPos, panelThickness, panelHeight, panelWidth, panelColor);
        DrawCubeWires(panelPos, panelThickness, panelHeight, panelWidth, trimColor);
    }

    if (IsOpenTileForWallFace(GetMapTileSafe(x + 1, y)))
    {
        Vector3 panelPos = tilePos;
        panelPos.x += WORLD_SCALE * 0.505f;
        panelPos.y = mapPosition.y + 2.0f * WORLD_SCALE;

        DrawCube(panelPos, panelThickness, panelHeight, panelWidth, panelColor);
        DrawCubeWires(panelPos, panelThickness, panelHeight, panelWidth, trimColor);
    }

    Vector3 bottomBand = tilePos;
    bottomBand.y = mapPosition.y + 0.18f * WORLD_SCALE;

    DrawCube(bottomBand, WORLD_SCALE * 1.02f, 0.12f * WORLD_SCALE, WORLD_SCALE * 1.02f, trimColor);

    Vector3 topBand = tilePos;
    topBand.y = mapPosition.y + 3.32f * WORLD_SCALE;

    DrawCube(topBand, WORLD_SCALE * 1.02f, 0.12f * WORLD_SCALE, WORLD_SCALE * 1.02f, trimColor);
}

static void DrawDoorTile(int x, int y, Vector3 tilePos)
{
    tilePos.y = mapPosition.y
        + (3.5f * WORLD_SCALE / 2.0f)
        + (shutterOpenTimer * 4.5f * WORLD_SCALE);

    float shutterWidthX = WORLD_SCALE * 0.96f;
    float shutterDepthZ = 0.25f;

    if (y > 0 && y < MAP_HEIGHT - 1)
    {
        if (IsDoorSideTile(myNewMap[y - 1][x]) || IsDoorSideTile(myNewMap[y + 1][x]))
        {
            shutterWidthX = 0.25f;
            shutterDepthZ = WORLD_SCALE * 0.96f;
        }
    }

    DrawCube(tilePos, shutterWidthX, 3.5f * WORLD_SCALE, shutterDepthZ, (Color){ 115, 72, 35, 255 });
    DrawCubeWires(tilePos, shutterWidthX, 3.5f * WORLD_SCALE, shutterDepthZ, DARKBROWN);

    Vector3 stripePos = tilePos;
    stripePos.y = tilePos.y + 0.5f * WORLD_SCALE;

    DrawCube(stripePos, shutterWidthX * 1.02f, 0.12f * WORLD_SCALE, shutterDepthZ * 1.02f, BLACK);
}

static void DrawDesk(Vector3 tilePos)
{
    Vector3 topPos = tilePos;
    topPos.y = mapPosition.y + 1.0f;

    DrawCube(topPos, WORLD_SCALE * 0.95f, 0.18f * WORLD_SCALE, WORLD_SCALE * 0.58f, BROWN);
    DrawCubeWires(topPos, WORLD_SCALE * 0.95f, 0.18f * WORLD_SCALE, WORLD_SCALE * 0.58f, DARKBROWN);

    Vector3 leg1 = { tilePos.x - 0.70f, mapPosition.y + 0.45f, tilePos.z - 0.35f };
    Vector3 leg2 = { tilePos.x + 0.70f, mapPosition.y + 0.45f, tilePos.z - 0.35f };
    Vector3 leg3 = { tilePos.x - 0.70f, mapPosition.y + 0.45f, tilePos.z + 0.35f };
    Vector3 leg4 = { tilePos.x + 0.70f, mapPosition.y + 0.45f, tilePos.z + 0.35f };

    DrawCube(leg1, 0.12f * WORLD_SCALE, 0.8f * WORLD_SCALE, 0.12f * WORLD_SCALE, DARKBROWN);
    DrawCube(leg2, 0.12f * WORLD_SCALE, 0.8f * WORLD_SCALE, 0.12f * WORLD_SCALE, DARKBROWN);
    DrawCube(leg3, 0.12f * WORLD_SCALE, 0.8f * WORLD_SCALE, 0.12f * WORLD_SCALE, DARKBROWN);
    DrawCube(leg4, 0.12f * WORLD_SCALE, 0.8f * WORLD_SCALE, 0.12f * WORLD_SCALE, DARKBROWN);
}

static void DrawTable(Vector3 tilePos)
{
    Vector3 topPos = tilePos;
    topPos.y = mapPosition.y + 1.0f;

    DrawCube(topPos, WORLD_SCALE * 1.25f, 0.2f * WORLD_SCALE, WORLD_SCALE * 0.95f, DARKBROWN);
    DrawCubeWires(topPos, WORLD_SCALE * 1.25f, 0.2f * WORLD_SCALE, WORLD_SCALE * 0.95f, BLACK);

    Vector3 basePos = tilePos;
    basePos.y = mapPosition.y + 0.45f;

    DrawCube(basePos, WORLD_SCALE * 0.28f, 0.8f * WORLD_SCALE, WORLD_SCALE * 0.28f, BROWN);
    DrawCubeWires(basePos, WORLD_SCALE * 0.28f, 0.8f * WORLD_SCALE, WORLD_SCALE * 0.28f, DARKBROWN);
}

static void DrawBarrel(Vector3 tilePos)
{
    tilePos.y = mapPosition.y + 0.8f;

    DrawCylinder(tilePos, 0.38f * WORLD_SCALE, 0.38f * WORLD_SCALE, 1.35f * WORLD_SCALE, 16, DARKGREEN);
    DrawCylinderWires(tilePos, 0.38f * WORLD_SCALE, 0.38f * WORLD_SCALE, 1.35f * WORLD_SCALE, 16, BLACK);

    Vector3 band = tilePos;
    band.y += 0.25f * WORLD_SCALE;
    DrawCylinder(band, 0.40f * WORLD_SCALE, 0.40f * WORLD_SCALE, 0.08f * WORLD_SCALE, 16, BLACK);
}

static void DrawLamp(Vector3 tilePos)
{
    Vector3 basePos = tilePos;
    basePos.y = mapPosition.y + 0.35f;

    DrawCylinder(basePos, 0.15f * WORLD_SCALE, 0.15f * WORLD_SCALE, 0.7f * WORLD_SCALE, 12, DARKGRAY);

    Vector3 polePos = tilePos;
    polePos.y = mapPosition.y + 1.2f;

    DrawCylinder(polePos, 0.07f * WORLD_SCALE, 0.07f * WORLD_SCALE, 1.6f * WORLD_SCALE, 12, GRAY);

    Vector3 lightPos = tilePos;
    lightPos.y = mapPosition.y + 2.2f;

    DrawSphere(lightPos, 0.25f * WORLD_SCALE, YELLOW);
    DrawSphereWires(lightPos, 0.25f * WORLD_SCALE, 12, 12, GOLD);
}

static bool CheckMapCollision(Vector3 testPos, float radius)
{
    Vector2 pos2D = { testPos.x, testPos.z };

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
                    int tileType = myNewMap[y][x];

                    if (IsSolidTile(tileType))
                    {
                        Rectangle wallRect = {
                            mapPosition.x + x * WORLD_SCALE,
                            mapPosition.z + y * WORLD_SCALE,
                            WORLD_SCALE,
                            WORLD_SCALE
                        };

                        if (CheckCollisionCircleRec(pos2D, radius, wallRect))
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

static void UpdateBody(Body *body, float rot, char side, char forward, bool jumpPressed, bool crouchHold)
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

    Vector3 front = (Vector3){ sinf(rot), 0.0f, cosf(rot) };
    Vector3 right = (Vector3){ cosf(-rot), 0.0f, sinf(-rot) };

    Vector3 desiredDir = (Vector3){
        input.x * right.x + input.y * front.x,
        0.0f,
        input.x * right.z + input.y * front.z
    };

    body->dir = Vector3Lerp(body->dir, desiredDir, CONTROL * delta);

    float decel = (body->isGrounded ? FRICTION : AIR_DRAG);
    Vector3 hvel = (Vector3){ body->velocity.x * decel, 0.0f, body->velocity.z * decel };

    float hvelLength = Vector3Length(hvel);

    if (hvelLength < (MAX_SPEED * 0.01f))
    {
        hvel = (Vector3){ 0 };
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

    if (!CheckMapCollision(nextPosX, playerRadius))
    {
        body->position.x = nextPosX.x;
    }
    else
    {
        body->velocity.x = 0.0f;
    }

    Vector3 nextPosZ = body->position;
    nextPosZ.z += body->velocity.z * delta;

    if (!CheckMapCollision(nextPosZ, playerRadius))
    {
        body->position.z = nextPosZ.z;
    }
    else
    {
        body->velocity.z = 0.0f;
    }

    body->position.y += body->velocity.y * delta;

    if (body->position.y <= 0.0f)
    {
        body->position.y = 0.0f;
        body->velocity.y = 0.0f;
        body->isGrounded = true;
    }
}

static void UpdateCameraFPS(Camera *camera)
{
    const Vector3 up = (Vector3){ 0.0f, 1.0f, 0.0f };
    const Vector3 targetOffset = (Vector3){ 0.0f, 0.0f, -1.0f };

    Vector3 yaw = Vector3RotateByAxisAngle(targetOffset, up, lookRotation.x);
    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    float pitchAngle = lookRotation.y - lean.y;
    pitchAngle = Clamp(pitchAngle, -89.0f * DEG2RAD, 89.0f * DEG2RAD);

    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

    float headSin = sinf(headTimer * PI);
    float headCos = cosf(headTimer * PI);

    const float stepRotation = 0.01f;
    camera->up = Vector3RotateByAxisAngle(up, pitch, headSin * stepRotation + lean.x);

    const float bobSide = 0.05f;
    const float bobUp = 0.08f;

    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walkLerp));
    camera->target = Vector3Add(camera->position, pitch);
}

static void DrawMiniMap(void)
{
    int scale = 12;
    int minimapX = GetScreenWidth() - (MAP_WIDTH * scale) - 30;
    int minimapY = 30;

    DrawRectangle(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, Fade(BLACK, 0.5f));

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int tile = myNewMap[y][x];

            if (tile == TILE_WALL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, DARKGRAY);
            else if (tile == TILE_SHUTTER) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BROWN);
            else if (tile == TILE_WALL_DECOR) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, MAROON);
            else if (tile == TILE_WALL_DARK) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BLACK);
            else if (tile == TILE_GOAL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, LIME);
            else if (tile == TILE_PILLAR) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, DARKGRAY);
            else if (tile == TILE_COVER) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BROWN);
            else if (tile == TILE_EXIT_SIGN) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, GREEN);
            else if (tile == TILE_WALL_BLUE) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BLUE);
            else if (tile == TILE_COLUMN_CYAN) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, SKYBLUE);
            else if (tile == TILE_DESK) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, BROWN);
            else if (tile == TILE_TABLE) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, DARKBROWN);
            else if (tile == TILE_WALL_FLAG) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, RED);
            else if (tile == TILE_BARREL) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, DARKGREEN);
            else if (tile == TILE_LAMP) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, YELLOW);
            else if (tile == TILE_PORTRAIT) DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, GOLD);
        }
    }

    DrawRectangleLines(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, GREEN);

    int playerCellX = (int)((player.position.x - mapPosition.x) / WORLD_SCALE);
    int playerCellY = (int)((player.position.z - mapPosition.z) / WORLD_SCALE);

    if (playerCellX >= 0 && playerCellX < MAP_WIDTH && playerCellY >= 0 && playerCellY < MAP_HEIGHT)
    {
        DrawRectangle(minimapX + playerCellX * scale, minimapY + playerCellY * scale, scale, scale, GREEN);
    }
}

static void DrawUI(void)
{
    DrawRectangle(5, 5, 520, 120, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 520, 120, BLUE);

    DrawText("Map Design Mode", 15, 15, 16, BLACK);
    DrawText("- Move: W/S/A/D", 15, 38, 14, BLACK);
    DrawText("- Mouse: Look around", 15, 58, 14, BLACK);
    DrawText("- Door: Press [E]", 15, 78, 14, MAROON);
    DrawText("- Arena: Move around pillars and cover", 15, 98, 14, MAROON);

    if (isShutterOpen && shutterOpenTimer >= 1.0f)
    {
        DrawText(TextFormat("Door closing in: %.1f sec", 1.5f - shutterHoldTimer), 15, 130, 14, RED);
    }

    DrawFPS(10, GetScreenHeight() - 30);
}


