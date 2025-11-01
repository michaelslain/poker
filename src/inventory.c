#include "inventory.h"
#include "card.h"
#include "chip.h"
#include <stdlib.h>
#include <string.h>

void Inventory_Init(Inventory* inventory, int initialCapacity) {
    inventory->stacks = (ItemStack*)malloc(sizeof(ItemStack) * initialCapacity);
    inventory->stackCount = 0;
    inventory->capacity = initialCapacity;
}

bool Inventory_AddItem(Inventory* inventory, struct Item* item) {
    // Get the type string for this item (e.g., "card_spades_ace" or "chip_5")
    const char* itemType = item->base.base.getType((Object*)item);
    
    // Try to find existing stack with matching type
    for (int i = 0; i < inventory->stackCount; i++) {
        if (strcmp(itemType, inventory->stacks[i].typeString) == 0) {
            // Found matching stack, increment count
            inventory->stacks[i].count++;
            return true;
        }
    }
    
    // Create new stack
    if (inventory->stackCount >= inventory->capacity) {
        // Expand capacity
        int newCapacity = inventory->capacity * 2;
        ItemStack* newStacks = (ItemStack*)realloc(inventory->stacks, sizeof(ItemStack) * newCapacity);
        if (!newStacks) return false;
        inventory->stacks = newStacks;
        inventory->capacity = newCapacity;
    }
    
    // Add new stack
    inventory->stacks[inventory->stackCount].item = item;
    inventory->stacks[inventory->stackCount].count = 1;
    
    // Copy the type string (not just the pointer!)
    strncpy(inventory->stacks[inventory->stackCount].typeString, itemType, 63);
    inventory->stacks[inventory->stackCount].typeString[63] = '\0';
    
    inventory->stackCount++;
    return true;
}

bool Inventory_RemoveItem(Inventory* inventory, int stackIndex) {
    if (stackIndex < 0 || stackIndex >= inventory->stackCount) return false;
    
    // Decrement count
    inventory->stacks[stackIndex].count--;
    
    // If stack is empty, remove it
    if (inventory->stacks[stackIndex].count <= 0) {
        // Shift stacks down
        for (int i = stackIndex; i < inventory->stackCount - 1; i++) {
            inventory->stacks[i] = inventory->stacks[i + 1];
        }
        inventory->stackCount--;
    }
    
    return true;
}

void Inventory_Cleanup(Inventory* inventory) {
    free(inventory->stacks);
    inventory->stacks = NULL;
    inventory->stackCount = 0;
    inventory->capacity = 0;
}
