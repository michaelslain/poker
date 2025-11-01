#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>

// Forward declaration to avoid circular dependency
struct Item;

typedef struct {
    struct Item** items;       // Dynamic array of item pointers
    int count;                 // Current number of items
    int capacity;              // Maximum capacity
} Inventory;

void Inventory_Init(Inventory* inventory, int initialCapacity);
bool Inventory_AddItem(Inventory* inventory, struct Item* item);
bool Inventory_RemoveItem(Inventory* inventory, int index);
void Inventory_Cleanup(Inventory* inventory);

#endif
