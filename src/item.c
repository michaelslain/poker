#include "item.h"

void Item_Init(Item* item, Vector3 pos) {
    // Initialize with custom callback that will handle inventory addition
    Interactable_Init(&item->base, pos, Item_DefaultInteract);
}

void Item_DefaultInteract(Interactable* self) {
    // For now, just deactivate the item (will be removed from world)
    // The actual inventory addition happens in main.c where we have access to player
    self->isActive = false;
}

void Item_Draw(Item* item, bool isClosest) {
    // Default drawing - just use base interactable draw
    Interactable_Draw(&item->base, isClosest);
}

void Item_DrawIcon(Item* item, Rectangle destRect) {
    // Default icon - just a gray square
    (void)item;
    DrawRectangleRec(destRect, GRAY);
    DrawRectangleLinesEx(destRect, 2, DARKGRAY);
}
