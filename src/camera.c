#include "camera.h"
#include "raymath.h"
#include <math.h>

void GameCamera_Init(GameCamera* cam, Vector3 pos) {
    Object_Init(&cam->base, pos);

    cam->camera.position = pos;
    cam->camera.target = (Vector3){pos.x, pos.y, pos.z - 1.0f};
    cam->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    cam->camera.fovy = 45.0f;
    cam->camera.projection = CAMERA_PERSPECTIVE;

    cam->angle = (Vector2){0.0f, 0.0f}; // pitch, yaw
}

void GameCamera_Update(GameCamera* cam, Vector2 mouseDelta) {
    // Update angles based on mouse movement
    float sensitivity = 0.003f;
    cam->angle.y += mouseDelta.x * sensitivity; // yaw
    cam->angle.x += mouseDelta.y * sensitivity; // pitch

    // Clamp pitch to prevent flipping
    if (cam->angle.x > 89.0f * DEG2RAD) cam->angle.x = 89.0f * DEG2RAD;
    if (cam->angle.x < -89.0f * DEG2RAD) cam->angle.x = -89.0f * DEG2RAD;

    // Calculate forward direction from angles
    Vector3 forward;
    forward.x = cosf(cam->angle.x) * sinf(cam->angle.y);
    forward.y = sinf(cam->angle.x);
    forward.z = cosf(cam->angle.x) * cosf(cam->angle.y);

    // Update camera target
    cam->camera.target = Vector3Add(cam->base.position, forward);
}

void GameCamera_SetTarget(GameCamera* cam, Vector3 target) {
    cam->base.position = target;
    cam->camera.position = target;
}

void GameCamera_AdjustFOV(GameCamera* cam) {
    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
        cam->camera.fovy += 5.0f;
        if (cam->camera.fovy > 120.0f) cam->camera.fovy = 120.0f;
    }
    if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
        cam->camera.fovy -= 5.0f;
        if (cam->camera.fovy < 20.0f) cam->camera.fovy = 20.0f;
    }
}
