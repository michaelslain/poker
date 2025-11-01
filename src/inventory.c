#include "inventory.h"
#include "item.h"
#include <stdlib.h>

void Inventory_Init(Inventory* inventory, int initialCapacity) {
    inventory->items = (struct Item**)malloc(sizeof(struct Item*) * initialCapacity);
    inventory->count = 0;
    inventory->capacity = initialCapacity;
}

bool Inventory_AddItem(Inventory* inventory, struct Item* item) {
    if (inventory->count >= inventory->capacity) {
        // Expand capacity
        int newCapacity = inventory->capacity * 2;
        struct Item** newItems = (struct Item**)realloc(inventory->items, sizeof(struct Item*) * newCapacity);
        if (!newItems) return false;
        
        inventory->items = newItems;
        inventory->capacity = newCapacity;
    }
    
    inventory->items[inventory->count] = item;
    inventory->count++;
    return true;
}

bool Inventory_RemoveItem(Inventory* inventory, int index) {
    if (index < 0 || index >= inventory->count) return false;
    
    // Shift items down
    for (int i = index; i < inventory->count - 1; i++) {
        inventory->items[i] = inventory->items[i + 1];
    }
    
    inventory->count--;
    return true;
}

void Inventory_Cleanup(Inventory* inventory) {
    free(inventory->items);
    inventory->items = NULL;
    inventory->count = 0;
    inventory->capacity = 0;
}
