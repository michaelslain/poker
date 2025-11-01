#include "interactable.h"
#include "raymath.h"

void Interactable_Init(Interactable* interactable, Vector3 pos, InteractCallback callback) {
    Object_Init(&interactable->base, pos);
    interactable->interactRange = 3.0f;
    interactable->isActive = true;
    interactable->onInteract = callback ? callback : Interactable_DefaultInteract;
}

void Interactable_Draw(Interactable* interactable, bool isClosest) {
    if (!interactable->isActive) return;
    
    // Draw a cube at the interactable's position
    Color color = isClosest ? YELLOW : BLUE;
    DrawCube(interactable->base.position, 0.5f, 0.5f, 0.5f, color);
    DrawCubeWires(interactable->base.position, 0.5f, 0.5f, 0.5f, BLACK);
}

void Interactable_Interact(Interactable* interactable) {
    if (interactable->isActive && interactable->onInteract) {
        interactable->onInteract(interactable);
    }
}

void Interactable_DefaultInteract(Interactable* self) {
    // Default behavior: deactivate (delete) the interactable
    self->isActive = false;
}
