#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "raylib.h"
#include "object.h"
#include <stdbool.h>

typedef struct Interactable Interactable;

typedef void (*InteractCallback)(Interactable* self);
typedef void (*DrawHighlightCallback)(Interactable* self, Camera3D camera);

struct Interactable {
    Object base;
    float interactRange;
    bool isActive;
    InteractCallback onInteract;
    DrawHighlightCallback onDrawHighlight;  // Custom highlight drawing for derived types
};

void Interactable_Init(Interactable* interactable, Vector3 pos, InteractCallback callback);
void Interactable_Draw(Interactable* interactable, bool isClosest, Camera3D camera);
void Interactable_Interact(Interactable* interactable);

// Draw the "E" prompt above the interactable (billboard)
void Interactable_DrawPrompt(Interactable* interactable, Camera3D camera);

// Default interaction: delete the interactable
void Interactable_DefaultInteract(Interactable* self);

#endif
