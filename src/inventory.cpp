#include "inventory.hpp"
#include "item.hpp"
#include "card.hpp"
#include "chip.hpp"
#include <string.h>
#include <algorithm>

Inventory::Inventory() {
}

Inventory::~Inventory() {
    Cleanup();
}

bool Inventory::AddItem(Item* item) {
    if (item == nullptr) {
        return false;
    }
    
    // Get the type string for this item (e.g., "card_spades_ace" or "chip_5")
    std::string itemType = item->GetType();
    if (itemType.empty()) {
        return false;
    }
    
    // Pistols and cards don't stack (each is a unique object) - always create new slot
    if (itemType == "pistol" || itemType.substr(0, 5) == "card_") {
        stacks.push_back(ItemStack(item, 1, itemType));
        Sort();  // Sort after adding
        return true;
    }
    
    // Try to find existing stack with matching type for other items (chips)
    for (size_t i = 0; i < stacks.size(); i++) {
        if (stacks[i].typeString == itemType) {
            // Found matching stack, increment count
            stacks[i].count++;
            // No need to sort when stacking - position doesn't change
            return true;
        }
    }
    
    // Create new stack
    stacks.push_back(ItemStack(item, 1, itemType));
    
    // Sort inventory after adding
    Sort();
    
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

void Inventory::Sort() {
    // DISABLED: Sorting is causing crashes when accessing card->rank on potentially invalid pointers
    // This happens when a player has "cheating cards" (picked up from world) mixed with dealt cards
    // TODO: Investigate why card pointers become invalid and implement safer sorting
    TraceLog(LOG_INFO, "Inventory::Sort - Skipping sort to avoid crashes");
}
