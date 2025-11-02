#include "chip.hpp"
#include "rlgl.h"
#include "raymath.h"
#include <cstdio>

Chip::Chip(int chipValue, Vector3 pos, PhysicsWorld* physics)
    : Item(pos), value(chipValue), iconTextureLoaded(false), rigidBody(nullptr)
{
    color = GetColorFromValue(value);

    // Initialize physics
    if (physics) {
        float radius = 0.3f;
        float height = 0.1f;
        Vector3 chipSize = { radius * 2, height, radius * 2 };
        rigidBody = new RigidBody(pos);
        rigidBody->InitBox(physics, pos, chipSize, 0.02f);
    }

    // Create icon texture
    iconTexture = LoadRenderTexture(60, 60);
    iconTextureLoaded = true;

    BeginTextureMode(iconTexture);
        ClearBackground({40, 40, 40, 255});

        int centerX = 30;
        int centerY = 30;
        int chipRadius = 20;

        DrawCircle(centerX, centerY, chipRadius, color);
        DrawCircleLines(centerX, centerY, chipRadius, DARKGRAY);
        DrawCircleLines(centerX, centerY, chipRadius - 1, DARKGRAY);
    EndTextureMode();
}

Chip::~Chip() {
    if (iconTextureLoaded) {
        UnloadRenderTexture(iconTexture);
        iconTextureLoaded = false;
    }
    if (rigidBody) {
        delete rigidBody;
        rigidBody = nullptr;
    }
}

void Chip::Update(float deltaTime) {
    (void)deltaTime;
    if (!isActive) return;
    if (!rigidBody || !rigidBody->body) return;

    rigidBody->Update(deltaTime);
    position = rigidBody->position;
    rotation = rigidBody->rotation;
}

void Chip::Draw(Camera3D camera) {
    (void)camera;
    if (!isActive) return;

    float radius = 0.3f;
    float height = 0.1f;

    Matrix rotMatrix = rigidBody ? rigidBody->GetRotationMatrix() : MatrixIdentity();
    Matrix transMatrix = MatrixTranslate(position.x, position.y, position.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);

    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));

        DrawCylinder({0, 0, 0}, radius, radius, height, 16, color);
        DrawCylinderWires({0, 0, 0}, radius, radius, height, 16, DARKGRAY);
    rlPopMatrix();
}

void Chip::DrawIcon(Rectangle destRect) {
    if (!iconTextureLoaded) return;

    Rectangle sourceRec = { 0, 0, (float)iconTexture.texture.width, -(float)iconTexture.texture.height };
    DrawTexturePro(iconTexture.texture, sourceRec, destRect, {0, 0}, 0.0f, WHITE);
}

const char* Chip::GetType() const {
    static char typeBuffer[32];
    snprintf(typeBuffer, sizeof(typeBuffer), "chip_%d", value);
    return typeBuffer;
}

Color Chip::GetColorFromValue(int value) {
    if (value >= 100) return BLACK;
    if (value >= 25) return GREEN;
    if (value >= 10) return BLUE;
    if (value >= 5) return RED;
    return WHITE;
}
