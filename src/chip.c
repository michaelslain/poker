#include "chip.h"
#include "rlgl.h"
#include "raymath.h"
#include <stdio.h>

// Virtual function wrappers
static void Chip_UpdateVirtual(Object* self, float deltaTime) {
    (void)deltaTime;
    Chip_Update((Chip*)self);
}

static void Chip_DrawVirtual(Object* self, Camera3D camera) {
    Chip_Draw((Chip*)self, camera);
}

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
    
    // Set the virtual function pointers
    chip->base.base.base.getType = Chip_GetType;
    chip->base.base.base.update = Chip_UpdateVirtual;
    chip->base.base.base.draw = Chip_DrawVirtual;
    
    // Override callback if provided
    if (callback) {
        chip->base.base.onInteract = callback;
    }
    
    // Set chip-specific properties
    chip->value = value;
    chip->color = Chip_GetColorFromValue(value);
    
    // Initialize physics FIRST (cylinder shape approximated as box for now)
    float radius = 0.3f;
    float height = 0.1f;
    Vector3 chipSize = { radius * 2, height, radius * 2 };
    RigidBody_InitBox(&chip->rigidBody, physics, pos, chipSize, 0.02f);
    
    // Create icon texture for this chip (60x60 square)
    chip->iconTexture = LoadRenderTexture(60, 60);
    chip->iconTextureLoaded = true;
    
    // Render the chip icon to the texture using simple 2D drawing
    BeginTextureMode(chip->iconTexture);
        ClearBackground((Color){40, 40, 40, 255});
        
        // Draw a simple 2D circle for the chip icon
        int centerX = 30;
        int centerY = 30;
        int chipRadius = 20;
        
        DrawCircle(centerX, centerY, chipRadius, chip->color);
        DrawCircleLines(centerX, centerY, chipRadius, DARKGRAY);
        DrawCircleLines(centerX, centerY, chipRadius - 1, DARKGRAY);
        
    EndTextureMode();
}

void Chip_Update(Chip* chip) {
    if (!chip->base.base.isActive) return;
    if (!chip->rigidBody.body) return;  // Skip if rigidbody not initialized
    
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
    if (!chip->iconTextureLoaded) return;
    
    // Draw the chip's icon texture as a flat 2D icon (flip vertically with negative height)
    Rectangle sourceRec = { 0, 0, (float)chip->iconTexture.texture.width, -(float)chip->iconTexture.texture.height };
    DrawTexturePro(chip->iconTexture.texture, sourceRec, destRect, (Vector2){0, 0}, 0.0f, WHITE);
}

void Chip_Cleanup(Chip* chip) {
    if (chip->iconTextureLoaded) {
        UnloadRenderTexture(chip->iconTexture);
        chip->iconTextureLoaded = false;
    }
    RigidBody_Cleanup(&chip->rigidBody);
}

const char* Chip_GetType(Object* obj) {
    Chip* chip = (Chip*)obj;
    static char typeBuffer[32];
    
    // Format: "chip_value"
    snprintf(typeBuffer, sizeof(typeBuffer), "chip_%d", chip->value);
    return typeBuffer;
}
