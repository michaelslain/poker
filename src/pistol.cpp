#include "pistol.hpp"
#include "rlgl.h"
#include "raymath.h"

Pistol::Pistol(Vector3 pos, PhysicsWorld* physics)
    : Item(pos), ammo(12), maxAmmo(12), rigidBody(nullptr)
{
    // Initialize physics if provided
    if (physics) {
        Vector3 pistolSize = {0.3f, 0.2f, 0.5f};  // Width, height, length
        rigidBody = new RigidBody(pos);
        rigidBody->InitBox(physics, pos, pistolSize, 0.5f);  // Moderate mass
    }
}

Pistol::~Pistol() {
    if (rigidBody) {
        delete rigidBody;
        rigidBody = nullptr;
    }
}

void Pistol::Update(float deltaTime) {
    (void)deltaTime;
    if (!isActive) return;
    
    // Only sync if physics is attached
    if (rigidBody && rigidBody->body) {
        rigidBody->Update(deltaTime);
        position = rigidBody->position;
        rotation = rigidBody->rotation;
    }
}

void Pistol::Draw(Camera3D camera) {
    (void)camera;
    if (!isActive) return;
    
    // Get rotation matrix from physics body
    Matrix rotMatrix = rigidBody ? rigidBody->GetRotationMatrix() : MatrixIdentity();
    Matrix transMatrix = MatrixTranslate(position.x, position.y, position.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    // Draw pistol with physics rotation
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        
        // Simple pistol shape: grip + barrel aligned properly
        // Grip (vertical handle)
        DrawCube({0, -0.1f, 0}, 0.08f, 0.25f, 0.12f, DARKGRAY);
        DrawCubeWires({0, -0.1f, 0}, 0.08f, 0.25f, 0.12f, BLACK);
        
        // Barrel (horizontal, extending forward from top of grip)
        DrawCube({0, 0.05f, 0.2f}, 0.06f, 0.06f, 0.4f, GRAY);
        DrawCubeWires({0, 0.05f, 0.2f}, 0.06f, 0.06f, 0.4f, BLACK);
    rlPopMatrix();
}

void Pistol::DrawIcon(Rectangle destRect) {
    // Draw pistol icon as simple rectangles
    DrawRectangleRec(destRect, DARKGRAY);
    
    // Draw barrel shape
    float barrelWidth = destRect.width * 0.6f;
    float barrelHeight = destRect.height * 0.3f;
    Rectangle barrelRect = {
        destRect.x + destRect.width * 0.2f,
        destRect.y + destRect.height * 0.2f,
        barrelWidth,
        barrelHeight
    };
    DrawRectangleRec(barrelRect, GRAY);
    
    DrawRectangleLinesEx(destRect, 2, BLACK);
}

void Pistol::DrawHeld(Camera3D camera) {
    // Draw pistol floating on the right side of the screen
    // Position it in view space relative to the camera
    
    Vector3 forward = Vector3Normalize({
        camera.target.x - camera.position.x,
        camera.target.y - camera.position.y,
        camera.target.z - camera.position.z
    });
    
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    Vector3 up = Vector3Normalize(camera.up);
    
    // Position pistol in front of camera, to the right, and slightly down
    Vector3 pistolPos = camera.position;
    pistolPos = Vector3Add(pistolPos, Vector3Scale(forward, 0.6f));   // Forward from camera
    pistolPos = Vector3Add(pistolPos, Vector3Scale(right, 0.35f));    // To the right
    pistolPos = Vector3Add(pistolPos, Vector3Scale(up, -0.25f));      // Down a bit
    
    // Build rotation matrix to align pistol with camera orientation
    // Pistol should point forward (along camera forward)
    Matrix rotMatrix = {
        right.x, right.y, right.z, 0,
        up.x, up.y, up.z, 0,
        forward.x, forward.y, forward.z, 0,
        0, 0, 0, 1
    };
    
    Matrix transMatrix = MatrixTranslate(pistolPos.x, pistolPos.y, pistolPos.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    // Draw the pistol
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        
        // Scale down for held view
        rlScalef(0.6f, 0.6f, 0.6f);
        
        // Don't rotate - the transform matrix already aligns it correctly
        // The barrel along +Z should point in the camera forward direction
        
        // Simple pistol shape: grip + barrel aligned properly
        // Grip (vertical handle)
        DrawCube({0, -0.1f, 0}, 0.08f, 0.25f, 0.12f, DARKGRAY);
        DrawCubeWires({0, -0.1f, 0}, 0.08f, 0.25f, 0.12f, BLACK);
        
        // Barrel (horizontal, extending forward from top of grip)
        DrawCube({0, 0.05f, 0.2f}, 0.06f, 0.06f, 0.4f, GRAY);
        DrawCubeWires({0, 0.05f, 0.2f}, 0.06f, 0.06f, 0.4f, BLACK);
    rlPopMatrix();
}

void Pistol::Shoot() {
    if (ammo > 0) {
        int oldAmmo = ammo;
        ammo--;
        TraceLog(LOG_INFO, "Pistol::Shoot() called - ammo changed from %d to %d", oldAmmo, ammo);
    }
}

const char* Pistol::GetType() const {
    return "pistol";
}
