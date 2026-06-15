#include "raylib.h"
#include "raymath.h"
#include "../../header/map.h"
#include "../../header/player.h"
#include "../../header/enemy.h"
#include <stdbool.h>

// 맵이 정확히 어디에 위치할 것인지에 대한 내용이다.
// 맵을 원점 기준 중앙에 오도록 배치한다.
Vector3 mapPosition = {
    -((MAP_WIDTH * WORLD_SCALE) / 2.0f),
    0.0f,
    -((MAP_HEIGHT * WORLD_SCALE) / 2.0f)
};

// 셔터 관련 전역 변수.
// 다른 파일에서 참조할 수 있으므로 유지한다.
bool isShutterOpen = false;
float shutterOpenTimer = 0.0f;
float shutterHoldTimer = 0.0f;

// map 배열 구조
// 이 2차원 배열이 실제 맵 설계도 역할을 한다.
// 0은 이동 공간, 10은 기둥, 11은 엄폐물이다.
// 최종 맵은 Wolfenstein식 방-복도 구조와 중앙 전투장 구조를 섞은 형태이다.
int myNewMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

    {1, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 1},

    {1, 13, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 13, 1},

    {1, 13, 0, 21, 0, 0, 3, 0, 13, 0, 15, 0, 2, 0, 20, 20, 0, 2, 0, 15, 0, 13, 0, 18, 0, 0, 3, 0, 13, 1},

    {1, 13, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 13, 1},

    {1, 13, 13, 13, 0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 13, 13, 13, 13, 13, 13, 13, 13, 0, 13, 13, 13, 13, 1},

    {1, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 1},

    {1, 13, 0, 13, 13, 13, 13, 13, 13, 5, 5, 5, 17, 17, 0, 0, 17, 17, 5, 5, 5, 13, 13, 13, 13, 13, 13, 0, 13, 1},

    {1, 13, 0, 13, 0, 0, 0, 0, 13, 5, 0, 11, 0, 0, 10, 10, 0, 0, 11, 0, 5, 13, 0, 0, 0, 0, 13, 0, 13, 1},

    {1, 13, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 13, 1},

    {1, 13, 0, 13, 0, 0, 0, 0, 13, 5, 0, 11, 0, 0, 16, 16, 0, 0, 11, 0, 5, 13, 0, 0, 0, 0, 13, 0, 13, 1},

    {1, 13, 0, 13, 0, 3, 0, 0, 13, 5, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 5, 13, 0, 0, 3, 0, 13, 0, 13, 1},

    {1, 13, 0, 13, 13, 13, 0, 13, 13, 5, 5, 5, 17, 17, 0, 0, 17, 17, 5, 5, 5, 13, 13, 13, 0, 13, 13, 0, 13, 1},

    {1, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 1},

    {1, 13, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 13, 0, 0, 13, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 13, 1},

    {1, 13, 0, 18, 0, 11, 0, 0, 13, 0, 15, 0, 2, 13, 0, 0, 13, 2, 0, 15, 0, 13, 0, 0, 11, 0, 18, 0, 13, 1},

    {1, 13, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 13, 1},

    {1, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 0, 9, 0, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 1},

    {1, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 1},

    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

static Vector3 GetTileCenterWorld(int x, int y);
static bool IsBlockingTile(int tileType);
static bool IsOpenTileForWallFace(int tileType);

static void DrawWolfWall(int x, int y, Vector3 tilePos, Color baseColor, Color panelColor, Color trimColor);
static void DrawDesk(Vector3 tilePos);
static void DrawTable(Vector3 tilePos);
static void DrawBarrel(Vector3 tilePos);
static void DrawLamp(Vector3 tilePos);
static void DrawEnemySpawnMarker(Vector3 tilePos);
static void DrawItemMarker(Vector3 tilePos);
static void DrawStartMarker(Vector3 tilePos);

Vector3 GetMapPosition(void)
{
    return mapPosition;
}

static Vector3 GetTileCenterWorld(int x, int y)
{
    return (Vector3){
        mapPosition.x + x * WORLD_SCALE + (WORLD_SCALE / 2.0f),
        mapPosition.y,
        mapPosition.z + y * WORLD_SCALE + (WORLD_SCALE / 2.0f)
    };
}

int GetMapTileSafe(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return TILE_WALL;
    }

    return myNewMap[y][x];
}

Vector3 GetPlayerStartPosition(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (myNewMap[y][x] == TILE_PLAYER_START)
            {
                Vector3 startPosition = GetTileCenterWorld(x, y);
                startPosition.y = 0.0f;
                return startPosition;
            }
        }
    }

    return (Vector3){ mapPosition.x + WORLD_SCALE, 0.0f, mapPosition.z + WORLD_SCALE };
}

void EnemeyPlayerSpawnPoint(void)
{
    bool playerSpawned = false;
    bool enemySpawned = false;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            Vector3 spawnPos = GetTileCenterWorld(x, y);

            if (myNewMap[y][x] == TILE_PLAYER_START && !playerSpawned)
            {
                player.position = (Vector3){ spawnPos.x, 0.0f, spawnPos.z };
                playerSpawned = true;
            }
            else if (myNewMap[y][x] == TILE_ENEMY && !enemySpawned)
            {
                InitEnemy(&enemy, (Vector3){ spawnPos.x, 1.5f, spawnPos.z });
                enemySpawned = true;
            }
        }
    }

    if (!playerSpawned)
    {
        player.position = GetPlayerStartPosition();
    }
}

// 오타 없는 이름도 같이 제공한다.
// 기존 코드가 EnemeyPlayerSpawnPoint를 호출해도 되고,
// 새 코드가 EnemyPlayerSpawnPoint를 호출해도 된다.
void EnemyPlayerSpawnPoint(void)
{
    EnemeyPlayerSpawnPoint();
}

static bool IsBlockingTile(int tileType)
{
    switch (tileType)
    {
        case TILE_WALL:
        case TILE_WALL_DECOR:
        case TILE_WALL_DARK:
        case TILE_PILLAR:
        case TILE_COVER:
        case TILE_EXIT_SIGN:
        case TILE_WALL_BLUE:
        case TILE_COLUMN_CYAN:
        case TILE_DESK:
        case TILE_TABLE:
        case TILE_WALL_FLAG:
        case TILE_BARREL:
        case TILE_LAMP:
        case TILE_PORTRAIT:
            return true;

        default:
            return false;
    }
}

static bool IsOpenTileForWallFace(int tileType)
{
    switch (tileType)
    {
        case TILE_EMPTY:
        case TILE_GOAL:
        case TILE_DESK:
        case TILE_TABLE:
        case TILE_COVER:
        case TILE_BARREL:
        case TILE_LAMP:
        case TILE_ENEMY:
        case TILE_ITEM:
        case TILE_PLAYER_START:
            return true;

        default:
            return false;
    }
}

bool CheckMapCollision(Vector3 testPos, float radius)
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

                    if (!IsBlockingTile(tileType))
                    {
                        continue;
                    }

                    float rectX = mapPosition.x + x * WORLD_SCALE;
                    float rectZ = mapPosition.z + y * WORLD_SCALE;
                    float rectW = WORLD_SCALE;
                    float rectH = WORLD_SCALE;

                    if (tileType == TILE_COVER)
                    {
                        rectW = WORLD_SCALE * 0.82f;
                        rectH = WORLD_SCALE * 0.82f;
                        rectX += (WORLD_SCALE - rectW) / 2.0f;
                        rectZ += (WORLD_SCALE - rectH) / 2.0f;

                        float coverSurfaceHeight = 0.68f * WORLD_SCALE;
                        float requiredJumpHeight = coverSurfaceHeight - 0.35f;

                        if (testPos.y >= requiredJumpHeight)
                        {
                            continue;
                        }
                    }
                    else if (tileType == TILE_DESK)
                    {
                        rectW = WORLD_SCALE * 0.95f;
                        rectH = WORLD_SCALE * 0.58f;
                        rectX += (WORLD_SCALE - rectW) / 2.0f;
                        rectZ += (WORLD_SCALE - rectH) / 2.0f;

                        float deskSurfaceHeight = mapPosition.y + 1.0f + ((0.18f * WORLD_SCALE) / 2.0f);
                        float requiredJumpHeight = deskSurfaceHeight - 0.30f;

                        if (testPos.y >= requiredJumpHeight)
                        {
                            continue;
                        }
                    }
                    else if (tileType == TILE_TABLE)
                    {
                        rectW = WORLD_SCALE * 1.25f;
                        rectH = WORLD_SCALE * 0.95f;
                        rectX += (WORLD_SCALE - rectW) / 2.0f;
                        rectZ += (WORLD_SCALE - rectH) / 2.0f;

                        float tableSurfaceHeight = mapPosition.y + 1.0f + ((0.20f * WORLD_SCALE) / 2.0f);
                        float requiredJumpHeight = tableSurfaceHeight - 0.30f;

                        if (testPos.y >= requiredJumpHeight)
                        {
                            continue;
                        }
                    }

                    Rectangle objectRect = {
                        rectX,
                        rectZ,
                        rectW,
                        rectH
                    };

                    if (CheckCollisionCircleRec(pos2D, radius, objectRect))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static void ApplyRaisedFloorForRect(
    Vector2 pos2D,
    float radius,
    Rectangle objectRect,
    float objectHeight,
    float requiredPlayerHeight,
    float playerY,
    float *floorHeight
)
{
    if (playerY < requiredPlayerHeight)
    {
        return;
    }

    if (CheckCollisionCircleRec(pos2D, radius, objectRect))
    {
        if (objectHeight > *floorHeight)
        {
            *floorHeight = objectHeight;
        }
    }
}
float GetMapFloorHeight(Vector3 testPos, float radius)
{
    float floorHeight = 0.0f;

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

                    float rectX = mapPosition.x + x * WORLD_SCALE;
                    float rectZ = mapPosition.z + y * WORLD_SCALE;
                    float rectW = WORLD_SCALE;
                    float rectH = WORLD_SCALE;

                    if (tileType == TILE_COVER)
                    {
                        rectW = WORLD_SCALE * 0.82f;
                        rectH = WORLD_SCALE * 0.82f;

                        rectX += (WORLD_SCALE - rectW) / 2.0f;
                        rectZ += (WORLD_SCALE - rectH) / 2.0f;

                        Rectangle coverRect = {
                            rectX,
                            rectZ,
                            rectW,
                            rectH
                        };

                        float coverSurfaceHeight = 0.68f * WORLD_SCALE;
                        float requiredPlayerHeight = coverSurfaceHeight - 0.35f;

                        ApplyRaisedFloorForRect(
                            pos2D,
                            radius,
                            coverRect,
                            coverSurfaceHeight,
                            requiredPlayerHeight,
                            testPos.y,
                            &floorHeight
                        );
                    }
                    else if (tileType == TILE_DESK)
                    {
                        rectW = WORLD_SCALE * 0.95f;
                        rectH = WORLD_SCALE * 0.58f;

                        rectX += (WORLD_SCALE - rectW) / 2.0f;
                        rectZ += (WORLD_SCALE - rectH) / 2.0f;

                        Rectangle deskRect = {
                            rectX,
                            rectZ,
                            rectW,
                            rectH
                        };

                        float deskSurfaceHeight = mapPosition.y + 1.0f + ((0.18f * WORLD_SCALE) / 2.0f);
                        float requiredPlayerHeight = deskSurfaceHeight - 0.30f;

                        ApplyRaisedFloorForRect(
                            pos2D,
                            radius,
                            deskRect,
                            deskSurfaceHeight,
                            requiredPlayerHeight,
                            testPos.y,
                            &floorHeight
                        );
                    }
                    else if (tileType == TILE_TABLE)
                    {
                        rectW = WORLD_SCALE * 1.25f;
                        rectH = WORLD_SCALE * 0.95f;

                        rectX += (WORLD_SCALE - rectW) / 2.0f;
                        rectZ += (WORLD_SCALE - rectH) / 2.0f;

                        Rectangle tableRect = {
                            rectX,
                            rectZ,
                            rectW,
                            rectH
                        };

                        float tableSurfaceHeight = mapPosition.y + 1.0f + ((0.20f * WORLD_SCALE) / 2.0f);
                        float requiredPlayerHeight = tableSurfaceHeight - 0.30f;

                        ApplyRaisedFloorForRect(
                            pos2D,
                            radius,
                            tableRect,
                            tableSurfaceHeight,
                            requiredPlayerHeight,
                            testPos.y,
                            &floorHeight
                        );
                    }
                }
            }
        }
    }

    return floorHeight;
}

void DrawMap(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            Vector3 tilePos = GetTileCenterWorld(x, y);

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
                    float coverHeight = 0.68f * WORLD_SCALE;

                    tilePos.y = mapPosition.y + (coverHeight / 2.0f);

                    DrawCube(
                        tilePos,
                        WORLD_SCALE * 0.82f,
                        coverHeight,
                        WORLD_SCALE * 0.82f,
                        BROWN
                    );

                    DrawCubeWires(
                        tilePos,
                        WORLD_SCALE * 0.82f,
                        coverHeight,
                        WORLD_SCALE * 0.82f,
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

                case TILE_ENEMY:
                {
                    DrawEnemySpawnMarker(tilePos);
                    break;
                }

                case TILE_ITEM:
                {
                    // 노란 아이템 마커를 숨긴다.
                    // 기존 DrawItemMarker(tilePos)는 호출하지 않는다.
                    break;
                }

                case TILE_PLAYER_START:
                {
                    DrawStartMarker(tilePos);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
}

// 기존 팀 코드에서 MapRender()를 호출하고 있을 가능성이 있어서 유지한다.
// 실제 렌더링은 DrawMap()으로 통일한다.
void MapRender(void)
{
    Vector3 floorCenter = {
        mapPosition.x + (MAP_WIDTH * WORLD_SCALE) / 2.0f,
        mapPosition.y,
        mapPosition.z + (MAP_HEIGHT * WORLD_SCALE) / 2.0f
    };

    DrawPlane(
        floorCenter,
        (Vector2){ MAP_WIDTH * WORLD_SCALE, MAP_HEIGHT * WORLD_SCALE },
        DARKGRAY
    );

    DrawMap();
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
    Vector3 barrelPos = tilePos;
    barrelPos.y = mapPosition.y + 0.8f;

    DrawCylinder(barrelPos, 0.38f * WORLD_SCALE, 0.38f * WORLD_SCALE, 1.35f * WORLD_SCALE, 16, DARKGREEN);
    DrawCylinderWires(barrelPos, 0.38f * WORLD_SCALE, 0.38f * WORLD_SCALE, 1.35f * WORLD_SCALE, 16, BLACK);

    Vector3 band = barrelPos;
    band.y += 0.25f * WORLD_SCALE;

    DrawCylinder(band, 0.40f * WORLD_SCALE, 0.40f * WORLD_SCALE, 0.08f * WORLD_SCALE, 16, BLACK);
}

static void DrawLamp(Vector3 tilePos)
{
    Vector3 basePos = tilePos;
    basePos.y = mapPosition.y + 0.35f;

    DrawCylinder(basePos, 0.15f * WORLD_SCALE, 0.15f * WORLD_SCALE, 0.7f * WORLD_SCALE, 12, DARKGREEN);

    Vector3 polePos = tilePos;
    polePos.y = mapPosition.y + 1.2f;

    DrawCylinder(polePos, 0.07f * WORLD_SCALE, 0.07f * WORLD_SCALE, 1.6f * WORLD_SCALE, 12, GREEN);

    Vector3 lightPos = tilePos;
    lightPos.y = mapPosition.y + 2.2f;

    DrawSphere(lightPos, 0.25f * WORLD_SCALE, YELLOW);
    DrawSphereWires(lightPos, 0.25f * WORLD_SCALE, 12, 12, GOLD);
}

static void DrawEnemySpawnMarker(Vector3 tilePos)
{
    tilePos.y = mapPosition.y + 0.08f;

    DrawCube(tilePos, WORLD_SCALE * 0.7f, 0.12f, WORLD_SCALE * 0.7f, RED);
    DrawCubeWires(tilePos, WORLD_SCALE * 0.7f, 0.12f, WORLD_SCALE * 0.7f, MAROON);

    Vector3 markerTop = tilePos;
    markerTop.y = mapPosition.y + 0.45f;

    DrawSphere(markerTop, WORLD_SCALE * 0.18f, RED);
}

static void DrawItemMarker(Vector3 tilePos)
{
    tilePos.y = mapPosition.y + 0.25f;

    DrawCube(tilePos, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, GOLD);
    DrawCubeWires(tilePos, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, ORANGE);
}

static void DrawStartMarker(Vector3 tilePos)
{
    tilePos.y = mapPosition.y + 0.06f;

    DrawCube(tilePos, WORLD_SCALE * 0.8f, 0.08f, WORLD_SCALE * 0.8f, GREEN);
    DrawCubeWires(tilePos, WORLD_SCALE * 0.8f, 0.08f, WORLD_SCALE * 0.8f, LIME);
}

void DrawMiniMap(Vector3 playerPosition)
{
    int scale = 8;
    int minimapX = GetScreenWidth() - (MAP_WIDTH * scale) - 20;
    int minimapY = 20;

    DrawRectangle(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, Fade(BLACK, 0.5f));

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int tile = myNewMap[y][x];
            Color color = BLANK;

            if (tile == TILE_WALL) color = DARKGRAY;
            else if (tile == TILE_WALL_DECOR) color = MAROON;
            else if (tile == TILE_WALL_DARK) color = BLACK;
            else if (tile == TILE_GOAL) color = LIME;
            else if (tile == TILE_PILLAR) color = DARKGRAY;
            else if (tile == TILE_COVER) color = BROWN;
            else if (tile == TILE_EXIT_SIGN) color = GREEN;
            else if (tile == TILE_WALL_BLUE) color = BLUE;
            else if (tile == TILE_COLUMN_CYAN) color = SKYBLUE;
            else if (tile == TILE_DESK) color = BROWN;
            else if (tile == TILE_TABLE) color = DARKBROWN;
            else if (tile == TILE_WALL_FLAG) color = RED;
            else if (tile == TILE_BARREL) color = DARKGREEN;
            else if (tile == TILE_LAMP) color = GREEN;
            else if (tile == TILE_PORTRAIT) color = GOLD;
            else if (tile == TILE_ENEMY) color = RED;
            else if (tile == TILE_PLAYER_START) color = GREEN;

            if (color.a > 0)
            {
                DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, color);
            }
        }
    }

    DrawRectangleLines(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, GREEN);

    int playerCellX = (int)((playerPosition.x - mapPosition.x) / WORLD_SCALE);
    int playerCellY = (int)((playerPosition.z - mapPosition.z) / WORLD_SCALE);

    if (playerCellX >= 0 && playerCellX < MAP_WIDTH &&
        playerCellY >= 0 && playerCellY < MAP_HEIGHT)
    {
        DrawRectangle(minimapX + playerCellX * scale, minimapY + playerCellY * scale, scale, scale, LIME);
    }
}