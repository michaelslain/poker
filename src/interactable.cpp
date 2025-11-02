#include "interactable.hpp"
#include "render_utils.hpp"
#include "raymath.h"

Interactable::Interactable(Vector3 pos)
    : Object(pos), interactRange(3.0f)
{
}

void Interactable::Interact() {
    if (isActive && onInteract) {
        onInteract(this);
    }
}

void Interactable::DrawPrompt(Camera3D camera) {
    // Position prompt above the object
    Vector3 promptPos = { position.x, position.y + 1.0f, position.z };
    
    // Draw a small white square background
    float squareSize = 0.3f;
    DrawBillboard(camera, (Texture2D){0, 0, 0, 0, 0}, promptPos, squareSize, WHITE);
    
    // Draw "E" text on top
    DrawTextBillboard(camera, "E", promptPos, 20, BLACK);
}

const char* Interactable::GetType() const {
    return "interactable";
}
