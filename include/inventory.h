#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>

// Forward declaration to avoid circular dependency
struct Item;

typedef struct {
    struct Item* item;         // First item in stack
    int count;                 // Number of items in stack
    char typeString[64];       // Full type identifier (e.g., "card_spades_ace" or "chip_5")
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

#endif
