#include "inventory.hpp"
#include "item.hpp"
#include <string.h>

Inventory::Inventory() {
}

Inventory::~Inventory() {
    Cleanup();
}

bool Inventory::AddItem(Item* item) {
    if (item == nullptr) return false;
    
    // Get the type string for this item (e.g., "card_spades_ace" or "chip_5")
    const char* itemType = item->GetType();
    
    // Try to find existing stack with matching type
    for (size_t i = 0; i < stacks.size(); i++) {
        if (stacks[i].typeString == itemType) {
            // Found matching stack, increment count
            stacks[i].count++;
            return true;
        }
    }
    
    // Create new stack
    stacks.push_back(ItemStack(item, 1, itemType));
    return true;
}

bool Inventory::RemoveItem(int stackIndex) {
    if (stackIndex < 0 || stackIndex >= (int)stacks.size()) return false;
    
    // Decrement count
    stacks[stackIndex].count--;
    
    // If stack is empty, remove it
    if (stacks[stackIndex].count <= 0) {
        stacks.erase(stacks.begin() + stackIndex);
    }
    
    return true;
}

void Inventory::Cleanup() {
    stacks.clear();
}
