#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "object.h"
#include "camera.h"

typedef struct {
    Object base;
    GameCamera camera;
    float speed;
    float lookYaw;
    float lookPitch;
} Player;

void Player_Init(Player* player, Vector3 pos);
void Player_Update(Player* player, float deltaTime);
Vector3 Player_GetPosition(Player* player);
Camera3D* Player_GetCamera(Player* player);

#endif
