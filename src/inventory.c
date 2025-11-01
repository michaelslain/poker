#include "inventory.h"
#include "item.h"
#include "card.h"
#include "chip.h"
#include <stdlib.h>
#include <string.h>

ItemType Inventory_GetItemType(struct Item* item) {
    // Try to cast to Chip and check if it has a valid color
    // Chips will have a color field that matches their value
    Chip* chip = (Chip*)item;
    
    // Check if this looks like a chip by checking if color matches expected values
    // and value is one of the valid chip values
    if (chip->value == 1 || chip->value == 5 || chip->value == 10 || 
        chip->value == 25 || chip->value == 100) {
        // Additional validation: check if color makes sense
        Color expectedColor = Chip_GetColorFromValue(chip->value);
        if (chip->color.r == expectedColor.r && 
            chip->color.g == expectedColor.g && 
            chip->color.b == expectedColor.b) {
            return ITEM_TYPE_CHIP;
        }
    }
    
    // Otherwise it's a card
    return ITEM_TYPE_CARD;
}

void Inventory_Init(Inventory* inventory, int initialCapacity) {
    inventory->stacks = (ItemStack*)malloc(sizeof(ItemStack) * initialCapacity);
    inventory->stackCount = 0;
    inventory->capacity = initialCapacity;
}

bool Inventory_AddItem(Inventory* inventory, struct Item* item) {
    ItemType type = Inventory_GetItemType(item);
    
    // For chips, try to find existing stack with same value
    if (type == ITEM_TYPE_CHIP) {
        Chip* chip = (Chip*)item;
        
        // Look for existing stack with same value
        for (int i = 0; i < inventory->stackCount; i++) {
            if (inventory->stacks[i].type == ITEM_TYPE_CHIP &&
                inventory->stacks[i].chipValue == chip->value) {
                // Found matching stack, increment count
                inventory->stacks[i].count++;
                return true;
            }
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
    inventory->stacks[inventory->stackCount].type = type;
    
    if (type == ITEM_TYPE_CHIP) {
        Chip* chip = (Chip*)item;
        inventory->stacks[inventory->stackCount].chipValue = chip->value;
    } else {
        inventory->stacks[inventory->stackCount].chipValue = 0;
    }
    
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
