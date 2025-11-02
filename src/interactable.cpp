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
    
    // Draw "E" text (white)
    DrawTextBillboard(camera, "E", promptPos, 20, WHITE);
}

const char* Interactable::GetType() const {
    return "interactable";
}
