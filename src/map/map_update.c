#include "raylib.h"
#include "../../header/player.h"
#include "../../header/map.h"
#include "../../header/enemy.h"

#define MAP_WIDTH  30  
#define MAP_HEIGHT 20  

#define TILE_EMPTY   0
#define TILE_WALL    1
#define TILE_ENEMY   2
#define TILE_ITEM    3
#define TILE_SHUTTER 4
#define TILE_PILLAR  5  
#define TILE_DANGER  6  
#define TILE_GOAL    9

bool isShutterOpen = false;

int const myNewMap[MAP_HEIGHT][MAP_WIDTH] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,2,0,0,0,1,0,0,0,0,0,1,0,0,3,0,0,0,0,0,1},
    {1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,2,0,1,0,3,0,0,0,0,0,1,0,2,0,1,0,0,0,3,0,0,0,2,0,0,1,0,1},
    {1,0,0,0,1,0,0,0,5,0,0,0,1,0,0,0,1,0,5,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,4,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1},
    {1,0,1,3,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,1,6,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,6,0,0,0,0,3,0,1},
    {1,0,2,0,1,1,1,1,1,0,1,0,2,0,5,0,2,0,1,0,1,1,1,1,1,0,2,0,0,1},
    {1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,1,4,1,1,1,4,1,1,0,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,5,0,0,0,0,0,0,0,2,0,0,9,0,0,2,0,0,0,0,0,0,0,0,0,0,5,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void EnemeyPlayerSpawnPoint(){
    //플레이어의 실제 포지션 어디에 있을것인지를 보는거다
    bool playerSpawned = false;
    bool enemySpawned = false;

    for (int y = 0; y < MAP_HEIGHT; y++) 
    {
        for (int x = 0; x < MAP_WIDTH; x++) 
        {
            // 2D 배열 인덱스(x, y)를 3D 월드의 정확한 중앙 좌표로 변환하는 공식
            float worldX = mapPosition.x + (x * WORLD_SCALE) + (WORLD_SCALE / 2.0f);
            float worldZ = mapPosition.z + (y * WORLD_SCALE) + (WORLD_SCALE / 2.0f);

            // 1. 플레이어 스폰: 맵에서 가장 처음 발견하는 빈 공간(0)에 배치
            if (myNewMap[y][x] == TILE_EMPTY && !playerSpawned) 
            {
                player.position = (Vector3){ worldX, 0.0f, worldZ };
                playerSpawned = true;
            }
            // 2. 적 스폰: 맵 배열에 배치된 숫자 2(TILE_ENEMY) 위치에 배치
            else if (myNewMap[y][x] == TILE_ENEMY && !enemySpawned) 
            {
                InitEnemy(&enemy, (Vector3){ worldX, 1.5f, worldZ });
                enemySpawned = true;
            }
        }
    }
}

void MapRender(){
    Vector3 floorCenter = { 
	mapPosition.x + (MAP_WIDTH * WORLD_SCALE) / 2.0f, 
	mapPosition.y, 
	mapPosition.z + (MAP_HEIGHT * WORLD_SCALE) / 2.0f 
    };
    DrawPlane(floorCenter, (Vector2){ MAP_WIDTH * WORLD_SCALE, MAP_HEIGHT * WORLD_SCALE }, DARKGRAY);

    // 3D 오브젝트 배치 및 렌더링
    for (int y = 0; y < MAP_HEIGHT; y++) 
    {
	for (int x = 0; x < MAP_WIDTH; x++) 
	{
	    Vector3 wallPos = { 
		mapPosition.x + x * WORLD_SCALE + (WORLD_SCALE / 2.0f), 
		mapPosition.y, 
		mapPosition.z + y * WORLD_SCALE + (WORLD_SCALE / 2.0f) 
	    };

	    switch (myNewMap[y][x])
	    {
		case TILE_WALL: 
		    wallPos.y = mapPosition.y + (3.5f * WORLD_SCALE / 2.0f); 
		    DrawCube(wallPos, WORLD_SCALE, 3.5f * WORLD_SCALE, WORLD_SCALE, GRAY);              
		    DrawCubeWires(wallPos, WORLD_SCALE, 3.5f * WORLD_SCALE, WORLD_SCALE, DARKGRAY);     
		    break;

		case TILE_ENEMY: 
		    wallPos.y = mapPosition.y + 1.5f;
		    DrawCylinder(wallPos, 0.5f, 0.5f, 3.0f, 8, PURPLE);
		    DrawCylinderWires(wallPos, 0.5f, 0.5f, 3.0f, 8, DARKPURPLE);
		    break;

		case TILE_ITEM: 
		    wallPos.y = mapPosition.y + 0.6f;
		    DrawCube(wallPos, 0.8f, 0.8f, 0.8f, YELLOW);              
		    DrawCubeWires(wallPos, 0.8f, 0.8f, 0.8f, GOLD);     
		    break;

		case TILE_SHUTTER: 
		    wallPos.y = mapPosition.y + (3.5f * WORLD_SCALE / 2.0f) + (shutterOpenTimer * 4.5f * WORLD_SCALE); 
		    
		    float shutterWidthX = WORLD_SCALE * 0.96f;
		    float shutterDepthZ = 0.25f;

		    if (y > 0 && y < MAP_HEIGHT - 1) {
			if (myNewMap[y-1][x] == TILE_WALL || myNewMap[y+1][x] == TILE_WALL) {
			    shutterWidthX = 0.25f; 
			    shutterDepthZ = WORLD_SCALE * 0.96f; 
			}
		    }

		    // 방화 셔터 본체 그리기
		    DrawCube(wallPos, shutterWidthX, 3.5f * WORLD_SCALE, shutterDepthZ, BROWN);            
		    DrawCubeWires(wallPos, shutterWidthX, 3.5f * WORLD_SCALE, shutterDepthZ, DARKBROWN);

		    break;

		case TILE_GOAL: 
		    wallPos.y = mapPosition.y + 0.05f;
		    DrawCube(wallPos, WORLD_SCALE, 0.1f, WORLD_SCALE, LIME);              
		    break;
	    }
	}
    }
}
