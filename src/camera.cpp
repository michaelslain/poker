#include "camera.hpp"
#include "raymath.h"
#include <cmath>

GameCamera::GameCamera(Vector3 pos)
    : angle({0.0f, 0.0f})
{
    camera.position = pos;
    camera.target = {pos.x, pos.y, pos.z - 1.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void GameCamera::Update(Vector2 mouseDelta) {
    // Update angles based on mouse movement
    float sensitivity = 0.003f;
    angle.y += mouseDelta.x * sensitivity; // yaw
    angle.x += mouseDelta.y * sensitivity; // pitch

    // Clamp pitch to prevent flipping
    if (angle.x > 89.0f * DEG2RAD) angle.x = 89.0f * DEG2RAD;
    if (angle.x < -89.0f * DEG2RAD) angle.x = -89.0f * DEG2RAD;

    // Calculate forward direction from angles
    Vector3 forward;
    forward.x = cosf(angle.x) * sinf(angle.y);
    forward.y = sinf(angle.x);
    forward.z = cosf(angle.x) * cosf(angle.y);

    // Update camera target
    camera.target = Vector3Add(camera.position, forward);
}

void GameCamera::SetTarget(Vector3 target) {
    camera.position = target;
}

void GameCamera::AdjustFOV() {
    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
        camera.fovy += 5.0f;
        if (camera.fovy > 120.0f) camera.fovy = 120.0f;
    }
    if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
        camera.fovy -= 5.0f;
        if (camera.fovy < 20.0f) camera.fovy = 20.0f;
    }
}
