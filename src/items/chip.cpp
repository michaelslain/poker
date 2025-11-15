#include "items/chip.hpp"
#include "raymath.h"
#include "rlgl.h"
#include <cstdio>
#include <stdexcept>
#include <string>

Chip::Chip(int chipValue, Vector3 pos, PhysicsWorld* physics)
    : Item(pos), value(chipValue), iconTextureLoaded(false), rigidBody(nullptr)
{
    usesLighting = false;  // Chips render without lighting
    
    color = GetColorFromValue(value);

    // Initialize physics
    if (physics) {
        float radius = 0.1f;  // Even smaller radius (was 0.3f originally)
        float height = 0.03f;  // Even thinner height (was 0.1f originally)
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
    if (!rigidBody || !rigidBody->body) return;

    rigidBody->Update(deltaTime);
    position = rigidBody->position;
    rotation = rigidBody->rotation;
}

void Chip::Draw(Camera3D camera) {
    (void)camera;
    
    float radius = 0.1f;  // Even smaller radius (was 0.3f originally)
    float height = 0.03f;  // Even thinner height (was 0.1f originally)
    
    // Ensure color is correct (recalculate from value)
    Color baseColor = GetColorFromValue(value);
    
    // Simple manual lighting: darken based on distance from light source
    // Light is at (0, 4, 0) - hardcoded for now
    Vector3 lightPos = {0.0f, 4.0f, 0.0f};
    float distanceToLight = Vector3Distance(position, lightPos);
    
    // Simple attenuation
    float attenuation = 1.0f / (1.0f + 0.18f * distanceToLight + 0.12f * distanceToLight * distanceToLight);
    attenuation = fmaxf(0.3f, attenuation); // Minimum brightness of 30%
    
    // Apply attenuation to color
    Color drawColor = {
        (unsigned char)(baseColor.r * attenuation),
        (unsigned char)(baseColor.g * attenuation),
        (unsigned char)(baseColor.b * attenuation),
        baseColor.a
    };
    
    // Use rotation from rigid body if available, otherwise use Object rotation
    Matrix rotMatrix;
    if (rigidBody && rigidBody->body) {
        rotMatrix = rigidBody->GetRotationMatrix();
    } else {
        // Use the rotation from Object base class
        rotMatrix = MatrixRotateXYZ((Vector3){
            rotation.x * DEG2RAD,
            rotation.y * DEG2RAD,
            rotation.z * DEG2RAD
        });
    }
    Matrix transMatrix = MatrixTranslate(position.x, position.y, position.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        DrawCylinder({0, 0, 0}, radius, radius, height, 16, drawColor);
    rlPopMatrix();
}

void Chip::DrawIcon(Rectangle destRect) {
    if (!iconTextureLoaded) return;
    Rectangle sourceRec = { 0, 0, (float)iconTexture.texture.width, -(float)iconTexture.texture.height };
    DrawTexturePro(iconTexture.texture, sourceRec, destRect, {0, 0}, 0.0f, WHITE);
}

std::string Chip::GetType() const {
    static char typeBuffer[64];
    std::string base = Item::GetType();
    snprintf(typeBuffer, sizeof(typeBuffer), "%s_chip_%d", base.c_str(), value);
    return typeBuffer;
}

Color Chip::GetColorFromValue(int value) {
    // Check for exact valid chip values
    if (value == 100) return BLACK;
    if (value == 25) return GREEN;
    if (value == 10) return BLUE;
    if (value == 5) return RED;
    if (value == 1) return WHITE;
    // Invalid chip value - throw error
    throw std::invalid_argument("Invalid chip value: " + std::to_string(value));
}

Object* Chip::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Chip(value, newPos, physics);
}
