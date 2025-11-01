#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "object.h"

typedef struct {
    Object base;
    Camera3D camera;
    Vector2 angle; // x = pitch, y = yaw
} GameCamera;

void GameCamera_Init(GameCamera* cam, Vector3 pos);
void GameCamera_Update(GameCamera* cam, Vector2 mouseDelta);
void GameCamera_SetTarget(GameCamera* cam, Vector3 target);
void GameCamera_AdjustFOV(GameCamera* cam);

#endif
