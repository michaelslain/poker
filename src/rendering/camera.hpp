#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "raylib.h"

class GameCamera {
public:
    Camera3D camera;
    Vector2 angle; // x = pitch, y = yaw

    GameCamera(Vector3 pos = {0.0f, 0.0f, 0.0f});
    void Update(Vector2 mouseDelta);
    void AdjustFOV();
    void SetTarget(Vector3 target);
    Camera3D* GetCamera() { return &camera; }
};

#endif
