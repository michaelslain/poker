#ifndef ITEM_H
#define ITEM_H

#include "interactable.h"
#include "inventory.h"

// Forward declare Item
struct Item;

typedef struct Item {
    Interactable base;
    // getType is now in Object base class
} Item;

// Forward declare the interact callback that adds to inventory
void Item_DefaultInteract(Interactable* self);

void Item_Init(Item* item, Vector3 pos);
void Item_Draw(Item* item, bool isClosest, Camera3D camera);
void Item_DrawIcon(Item* item, Rectangle destRect);

#endif
