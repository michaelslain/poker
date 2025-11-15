#include "substances/substance.hpp"
#include "rlgl.h"
#include "raymath.h"

Substance::Substance(Vector3 pos, Color substanceColor, PhysicsWorld* physics)
    : Item(pos), rigidBody(nullptr), color(substanceColor)
{
    usable = true;  // Substances can be consumed
    // Initialize physics if provided
    if (physics) {
        Vector3 substanceSize = {0.2f, 0.2f, 0.2f};  // Small cube for substances
        rigidBody = new RigidBody(pos);
        rigidBody->InitBox(physics, pos, substanceSize, 0.1f);  // Light mass
    }
}

Substance::~Substance() {
    if (rigidBody) {
        delete rigidBody;
        rigidBody = nullptr;
    }
}

void Substance::Update(float deltaTime) {
    (void)deltaTime;
    
    // Only sync if physics is attached
    if (rigidBody && rigidBody->body) {
        rigidBody->Update(deltaTime);
        position = rigidBody->position;
        rotation = rigidBody->rotation;
    }
}

void Substance::Draw(Camera3D camera) {
    (void)camera;
    
    // Get rotation matrix from physics body
    Matrix rotMatrix = rigidBody ? rigidBody->GetRotationMatrix() : MatrixIdentity();
    Matrix transMatrix = MatrixTranslate(position.x, position.y, position.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    // Draw substance as a small cube with physics rotation
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        
        // Draw small cube representing the substance
        DrawCube({0, 0, 0}, 0.2f, 0.2f, 0.2f, color);
        DrawCubeWires({0, 0, 0}, 0.2f, 0.2f, 0.2f, BLACK);
    rlPopMatrix();
}

void Substance::DrawIcon(Rectangle destRect) {
    // Draw substance icon as colored rectangle
    DrawRectangleRec(destRect, color);
    DrawRectangleLinesEx(destRect, 2, BLACK);
    
    // Draw substance name
    const char* name = GetName();
    int fontSize = 10;
    int textWidth = MeasureText(name, fontSize);
    DrawText(name, 
             (int)(destRect.x + destRect.width / 2.0f - textWidth / 2.0f),
             (int)(destRect.y + destRect.height / 2.0f - fontSize / 2.0f),
             fontSize, WHITE);
}

std::string Substance::GetType() const {
    return Item::GetType() + "_substance";
}

void Substance::Use() {
    Consume();  // Call the substance-specific consume effect
}
