#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "raylib.h"
#include "object.h"
#include <stdbool.h>

typedef struct Interactable Interactable;

typedef void (*InteractCallback)(Interactable* self);

struct Interactable {
    Object base;
    float interactRange;
    bool isActive;
    InteractCallback onInteract;
};

void Interactable_Init(Interactable* interactable, Vector3 pos, InteractCallback callback);
void Interactable_Draw(Interactable* interactable, bool isClosest);
void Interactable_Interact(Interactable* interactable);

// Default interaction: delete the interactable
void Interactable_DefaultInteract(Interactable* self);

#endif
