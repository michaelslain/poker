#include "interactable.h"
#include "render_utils.h"
#include "raymath.h"
#include <stddef.h>

void Interactable_Init(Interactable* interactable, Vector3 pos, InteractCallback callback) {
    Object_Init(&interactable->base, pos);
    interactable->interactRange = 3.0f;
    interactable->isActive = true;
    interactable->onInteract = callback ? callback : Interactable_DefaultInteract;
    interactable->onDrawHighlight = NULL;  // Must be set by derived classes
}

void Interactable_Draw(Interactable* interactable, bool isClosest, Camera3D camera) {
    if (!interactable->isActive) return;
    
    // If closest, draw custom highlight (if callback is set)
    if (isClosest && interactable->onDrawHighlight) {
        interactable->onDrawHighlight(interactable, camera);
    }
    
    // If closest, draw the "E" prompt
    if (isClosest) {
        Interactable_DrawPrompt(interactable, camera);
    }
}

void Interactable_DrawPrompt(Interactable* interactable, Camera3D camera) {
    Vector3 pos = interactable->base.position;
    
    // Position prompt above the object (offset by 1 unit)
    Vector3 promptPos = { pos.x, pos.y + 1.0f, pos.z };
    
    // Draw a small white square background
    float squareSize = 0.3f;
    DrawBillboard(camera, (Texture2D){0}, promptPos, squareSize, WHITE);
    
    // Draw "E" text on top
    DrawTextBillboard(camera, "E", promptPos, 20, BLACK);
}

void Interactable_Interact(Interactable* interactable) {
    if (interactable->isActive && interactable->onInteract) {
        interactable->onInteract(interactable);
    }
}

void Interactable_DefaultInteract(Interactable* self) {
    // Default behavior: do nothing
    // Derived classes should override this callback
    (void)self;
}
