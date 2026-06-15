#include "raylib.h"

//struct
typedef struct {
    float health;
    Vector3 position;
    Vector3 velocity;
    Vector3 dir;
    bool isGrounded;
} Body;
//variable
extern Body player;

//functions
bool CheckMapCollision(Vector3 testPos, float radius);
void UpdateBody(Body *body, float rot, bool jumpPressed);
void PlayerMouseMovement();
