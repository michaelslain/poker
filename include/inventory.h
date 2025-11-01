#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>

// Forward declaration to avoid circular dependency
struct Item;

typedef enum {
    ITEM_TYPE_CARD,
    ITEM_TYPE_CHIP
} ItemType;

typedef struct {
    struct Item* item;         // First item in stack
    int count;                 // Number of items in stack
    ItemType type;             // Type of item
    int chipValue;             // For chips: the value (used for stacking by value)
} ItemStack;

typedef struct {
    ItemStack* stacks;         // Dynamic array of item stacks
    int stackCount;            // Number of stacks
    int capacity;              // Maximum capacity
} Inventory;

void Inventory_Init(Inventory* inventory, int initialCapacity);
bool Inventory_AddItem(Inventory* inventory, struct Item* item);
bool Inventory_RemoveItem(Inventory* inventory, int stackIndex);
void Inventory_Cleanup(Inventory* inventory);

// Helper to get item type
ItemType Inventory_GetItemType(struct Item* item);

#endif
