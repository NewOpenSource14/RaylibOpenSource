#include "raylib.h"
#include "raymath.h"
#include "../header/map.h"

// 맵이 정확히 어디에 위치할 것인지에 대한 내용이다.
// 맵을 원점 기준 중앙에 오도록 배치한다.
static Vector3 mapPosition = {
    -((MAP_WIDTH * WORLD_SCALE) / 2.0f),
    0.0f,
    -((MAP_HEIGHT * WORLD_SCALE) / 2.0f)
};

// map.c 배열 구조
// 이 2차원 배열이 실제 맵 설계도 역할을 한다.
// 0은 이동 공간, 10은 기둥, 11은 엄폐물이다.
// 최종 맵은 Wolfenstein식 방-복도 구조와 중앙 전투장 구조를 섞은 형태이다.
static int myMap[MAP_HEIGHT][MAP_WIDTH] = {
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

    return myMap[y][x];
}

Vector3 GetPlayerStartPosition(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (myMap[y][x] == TILE_PLAYER_START)
            {
                Vector3 startPosition = GetTileCenterWorld(x, y);
                startPosition.y = 0.0f;
                return startPosition;
            }
        }
    }

    // 만약 TILE_PLAYER_START를 찾지 못하면 기본 위치로 시작한다.
    return (Vector3){ mapPosition.x + WORLD_SCALE, 0.0f, mapPosition.z + WORLD_SCALE };
}

static bool IsBlockingTile(int tileType)
{
    // 충돌 처리
    // 벽, 기둥, 책상, 테이블, 드럼통 등을 충돌 타일로 처리한다.
    // 플레이어가 오브젝트를 뚫고 지나가지 못하도록 설계한다.
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
    // 벽의 옆면 패널을 그릴지 판단하는 함수이다.
    // 주변이 빈 공간이나 오브젝트이면 벽의 장식 면이 보이게 한다.
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
    // 좌표 평면만 보겠다 2D로 보겠다 이말이다.
    // 이것은 어찌보면 플레이어의 포지션이다.
    Vector2 pos2D = { testPos.x, testPos.z };

    // 현재 플레이어가 어느 맵 셀에 있는지 계산한다.
    int cellX = (int)((testPos.x - mapPosition.x) / WORLD_SCALE);
    int cellY = (int)((testPos.z - mapPosition.z) / WORLD_SCALE);

    // 플레이어 주변 3x3 타일만 검사해서 충돌 처리를 가볍게 한다.
    for (int y = cellY - 1; y <= cellY + 1; y++)
    {
        if (y >= 0 && y < MAP_HEIGHT)
        {
            for (int x = cellX - 1; x <= cellX + 1; x++)
            {
                if (x >= 0 && x < MAP_WIDTH)
                {
                    int tileType = myMap[y][x];
                    bool solid = IsBlockingTile(tileType);

                    if (solid)
                    {
                        float rectX = mapPosition.x + x * WORLD_SCALE;
                        float rectZ = mapPosition.z + y * WORLD_SCALE;
                        float rectW = WORLD_SCALE;
                        float rectH = WORLD_SCALE;

                        // 엄폐물은 몸을 가릴 정도로 보이지만, 점프하면 넘어갈 수 있도록 따로 처리한다.
                        if (tileType == TILE_COVER)
                        {
                            float jumpableHeight = 0.50f * WORLD_SCALE;

                            if (testPos.y > jumpableHeight)
                            {
                                continue;
                            }

                            rectW = WORLD_SCALE * 0.82f;
                            rectH = WORLD_SCALE * 0.82f;
                            rectX += (WORLD_SCALE - rectW) / 2.0f;
                            rectZ += (WORLD_SCALE - rectH) / 2.0f;
                        }

                        Rectangle wallRect = {
                            rectX,
                            rectZ,
                            rectW,
                            rectH
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

void DrawMap(void)
{
    // switch문으로 3D 오브젝트 변환
    // 배열을 한 칸씩 읽고 3D 좌표로 변환한다.
    // switch문으로 타일 번호에 맞는 오브젝트를 그린다.
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            Vector3 tilePos = GetTileCenterWorld(x, y);

            switch (myMap[y][x])
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
                    // 엄폐물은 몸은 가리지만 시야는 완전히 막지 않는 높이로 설정한다.
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
                    DrawItemMarker(tilePos);
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

static void DrawWolfWall(int x, int y, Vector3 tilePos, Color baseColor, Color panelColor, Color trimColor)
{
    // DrawWolfWall() 벽 표현
    // 회색 큐브 벽이 너무 밋밋해서 개선한 부분이다.
    // 벽의 기본색, 패널색, 테두리색을 따로 설정한다.
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
    // 정적 오브젝트 구현
    // 책상을 별도 함수로 구현한다.
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
    // 테이블을 별도 함수로 구현한다.
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
    // 드럼통을 별도 함수로 구현한다.
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
    // 램프를 별도 함수로 구현한다.
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

static void DrawEnemySpawnMarker(Vector3 tilePos)
{
    // 적 스폰 위치를 표시하는 마커이다.
    // 실제 적 AI나 공격 로직은 이 함수가 아니라 적 담당 코드에서 연결해야 한다.
    tilePos.y = mapPosition.y + 0.08f;

    DrawCube(tilePos, WORLD_SCALE * 0.7f, 0.12f, WORLD_SCALE * 0.7f, RED);
    DrawCubeWires(tilePos, WORLD_SCALE * 0.7f, 0.12f, WORLD_SCALE * 0.7f, MAROON);

    Vector3 markerTop = tilePos;
    markerTop.y = mapPosition.y + 0.45f;

    DrawSphere(markerTop, WORLD_SCALE * 0.18f, RED);
}

static void DrawItemMarker(Vector3 tilePos)
{
    // 아이템 위치를 표시하는 마커이다.
    tilePos.y = mapPosition.y + 0.25f;

    DrawCube(tilePos, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, GOLD);
    DrawCubeWires(tilePos, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, WORLD_SCALE * 0.35f, ORANGE);
}

static void DrawStartMarker(Vector3 tilePos)
{
    // 플레이어 시작 위치를 표시하는 마커이다.
    tilePos.y = mapPosition.y + 0.06f;

    DrawCube(tilePos, WORLD_SCALE * 0.8f, 0.08f, WORLD_SCALE * 0.8f, GREEN);
    DrawCubeWires(tilePos, WORLD_SCALE * 0.8f, 0.08f, WORLD_SCALE * 0.8f, LIME);
}

void DrawMiniMap(Vector3 playerPosition)
{
    // 2D 미니맵 레이더
    int scale = 8;
    int minimapX = GetScreenWidth() - (MAP_WIDTH * scale) - 20;
    int minimapY = 20;

    DrawRectangle(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, Fade(BLACK, 0.5f));

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int tile = myMap[y][x];
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
            else if (tile == TILE_LAMP) color = YELLOW;
            else if (tile == TILE_PORTRAIT) color = GOLD;
            else if (tile == TILE_ENEMY) color = RED;
            else if (tile == TILE_ITEM) color = ORANGE;
            else if (tile == TILE_PLAYER_START) color = GREEN;

            if (color.a > 0)
            {
                DrawRectangle(minimapX + x * scale, minimapY + y * scale, scale, scale, color);
            }
        }
    }

    // 이것은 맵의 테두리라고 볼 수 있다.
    DrawRectangleLines(minimapX, minimapY, MAP_WIDTH * scale, MAP_HEIGHT * scale, GREEN);

    // 미니맵 위 플레이어 위치이다.
    int playerCellX = (int)((playerPosition.x - mapPosition.x) / WORLD_SCALE);
    int playerCellY = (int)((playerPosition.z - mapPosition.z) / WORLD_SCALE);

    if (playerCellX >= 0 && playerCellX < MAP_WIDTH && playerCellY >= 0 && playerCellY < MAP_HEIGHT)
    {
        DrawRectangle(minimapX + playerCellX * scale, minimapY + playerCellY * scale, scale, scale, LIME);
    }
}