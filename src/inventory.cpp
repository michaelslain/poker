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
    if (item == nullptr) return false;
    
    // Get the type string for this item (e.g., "card_spades_ace" or "chip_5")
    const char* itemType = item->GetType();
    
    // Pistols don't stack (each has unique ammo) - always create new slot
    if (strcmp(itemType, "pistol") == 0) {
        stacks.push_back(ItemStack(item, 1, itemType));
        Sort();  // Sort after adding pistol
        return true;
    }
    
    // Try to find existing stack with matching type for other items
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
    // Use stable_sort to maintain relative order of equal elements
    std::stable_sort(stacks.begin(), stacks.end(), [](const ItemStack& a, const ItemStack& b) {
        const char* typeA = a.typeString.c_str();
        const char* typeB = b.typeString.c_str();
        
        // Determine categories
        bool aIsWeapon = (strncmp(typeA, "pistol", 6) == 0);
        bool bIsWeapon = (strncmp(typeB, "pistol", 6) == 0);
        bool aIsCard = (strncmp(typeA, "card_", 5) == 0);
        bool bIsCard = (strncmp(typeB, "card_", 5) == 0);
        bool aIsChip = (strncmp(typeA, "chip_", 5) == 0);
        bool bIsChip = (strncmp(typeB, "chip_", 5) == 0);
        
        // Category ordering: weapons < cards < chips
        // If one is a weapon and the other isn't, weapon comes first
        if (aIsWeapon && !bIsWeapon) return true;
        if (!aIsWeapon && bIsWeapon) return false;
        
        // If one is a card and the other isn't (and neither is a weapon), card comes first
        if (aIsCard && !bIsCard) return true;
        if (!aIsCard && bIsCard) return false;
        
        // Within cards: sort by rank (two to king, then ace)
        // Ace (RANK_ACE = 1) should be last (treated as 14)
        if (aIsCard && bIsCard) {
            Card* cardA = static_cast<Card*>(a.item);
            Card* cardB = static_cast<Card*>(b.item);
            if (cardA && cardB) {
                int rankA = (cardA->rank == RANK_ACE) ? 14 : cardA->rank;
                int rankB = (cardB->rank == RANK_ACE) ? 14 : cardB->rank;
                return rankA < rankB;
            }
        }
        
        // Within chips: sort by value (least to greatest)
        if (aIsChip && bIsChip) {
            Chip* chipA = static_cast<Chip*>(a.item);
            Chip* chipB = static_cast<Chip*>(b.item);
            if (chipA && chipB) {
                return chipA->value < chipB->value;
            }
        }
        
        // Default: maintain existing order
        return false;
    });
}
