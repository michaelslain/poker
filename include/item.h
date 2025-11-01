#ifndef ITEM_H
#define ITEM_H

#include "interactable.h"
#include "inventory.h"

typedef struct Item {
    Interactable base;
    // Items can have additional properties here in the future
} Item;

// Forward declare the interact callback that adds to inventory
void Item_DefaultInteract(Interactable* self);

void Item_Init(Item* item, Vector3 pos);
void Item_Draw(Item* item, bool isClosest);
void Item_DrawIcon(Item* item, Rectangle destRect);

#endif
