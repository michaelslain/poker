#include "chip.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>

Color Chip_GetColorFromValue(int value) {
    if (value >= 100) return BLACK;
    if (value >= 25) return GREEN;
    if (value >= 10) return BLUE;
    if (value >= 5) return RED;
    return WHITE;
}

void Chip_Init(Chip* chip, int value, Vector3 pos, InteractCallback callback, PhysicsWorld* physics) {
    // Initialize base item
    Item_Init(&chip->base, pos);
    
    // Override callback if provided
    if (callback) {
        chip->base.base.onInteract = callback;
    }
    
    // Set chip-specific properties
    chip->value = value;
    chip->color = Chip_GetColorFromValue(value);
    
    // Initialize physics (cylinder shape approximated as box for now)
    float radius = 0.3f;
    float height = 0.1f;
    Vector3 chipSize = { radius * 2, height, radius * 2 };
    RigidBody_InitBox(&chip->rigidBody, physics, pos, chipSize, 0.02f);
}

void Chip_Update(Chip* chip) {
    if (!chip->base.base.isActive) return;
    
    // Sync object position with physics body
    RigidBody_Update(&chip->rigidBody);
    chip->base.base.base.position = chip->rigidBody.base.position;
    chip->base.base.base.rotation = chip->rigidBody.base.rotation;
}

void Chip_Draw(Chip* chip, Camera3D camera) {
    (void)camera;
    if (!chip->base.base.isActive) return;
    
    Vector3 pos = chip->base.base.base.position;
    
    // Chip dimensions
    float radius = 0.3f;
    float height = 0.1f;
    
    // Get rotation matrix from physics body
    Matrix rotMatrix = RigidBody_GetRotationMatrix(&chip->rigidBody);
    Matrix transMatrix = MatrixTranslate(pos.x, pos.y, pos.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    // Draw cylinder with physics rotation
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        
        // Draw cylinder (approximated as stacked circles)
        DrawCylinder((Vector3){0, 0, 0}, radius, radius, height, 16, chip->color);
        DrawCylinderWires((Vector3){0, 0, 0}, radius, radius, height, 16, DARKGRAY);
    rlPopMatrix();
}

void Chip_DrawIcon(Chip* chip, Rectangle destRect) {
    // Render 3D chip model to icon
    
    // Create a simple orthographic camera for icon rendering
    Camera3D iconCamera = { 0 };
    iconCamera.position = (Vector3){ 0.5f, 0.5f, 0.5f };
    iconCamera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    iconCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    iconCamera.fovy = 45.0f;
    iconCamera.projection = CAMERA_PERSPECTIVE;
    
    // Set up scissor test to clip to icon area
    BeginScissorMode((int)destRect.x, (int)destRect.y, (int)destRect.width, (int)destRect.height);
    
    // Render 3D chip
    BeginMode3D(iconCamera);
        float radius = 0.15f;
        float height = 0.05f;
        
        // Draw cylinder chip
        DrawCylinder((Vector3){0, 0, 0}, radius, radius, height, 16, chip->color);
        DrawCylinderWires((Vector3){0, 0, 0}, radius, radius, height, 16, DARKGRAY);
    EndMode3D();
    
    EndScissorMode();
    
    // Draw border around icon
    DrawRectangleLinesEx(destRect, 2, DARKGRAY);
}

void Chip_Cleanup(Chip* chip) {
    RigidBody_Cleanup(&chip->rigidBody);
}
