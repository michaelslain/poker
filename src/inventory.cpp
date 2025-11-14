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
    
    // Non-stackable items (cards, weapons, etc.) - always create new slot
    if (!item->CanStack()) {
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
    // Use stable_sort to maintain relative order of equal elements
    std::stable_sort(stacks.begin(), stacks.end(), [](const ItemStack& a, const ItemStack& b) {
        // Safety check: ensure items are valid
        if (!a.item || !b.item) return false;
        
        const std::string& typeA = a.typeString;
        const std::string& typeB = b.typeString;

        // Determine categories
        bool aIsWeapon = (typeA.find("pistol") != std::string::npos);
        bool bIsWeapon = (typeB.find("pistol") != std::string::npos);
        bool aIsCard = (typeA.find("card") != std::string::npos);
        bool bIsCard = (typeB.find("card") != std::string::npos);
        bool aIsChip = (typeA.find("chip") != std::string::npos);
        bool bIsChip = (typeB.find("chip") != std::string::npos);

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
            // Extra safety: verify casts are valid before accessing rank
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
            // Extra safety: verify casts are valid before accessing value
            if (chipA && chipB) {
                return chipA->value < chipB->value;
            }
        }

        // Default: maintain existing order
        return false;
    });
}

int Inventory::CountItemsByType(const std::string& typeSubstring) const {
    int count = 0;
    for (size_t i = 0; i < stacks.size(); i++) {
        if (stacks[i].item && stacks[i].item->GetType().find(typeSubstring) != std::string::npos) {
            count += stacks[i].count;
        }
    }
    return count;
}

std::vector<int> Inventory::GetIndicesByType(const std::string& typeSubstring) const {
    std::vector<int> indices;
    for (size_t i = 0; i < stacks.size(); i++) {
        if (stacks[i].item && stacks[i].item->GetType().find(typeSubstring) != std::string::npos) {
            indices.push_back(i);
        }
    }
    return indices;
}

int Inventory::GetTotalChipValue() const {
    int totalValue = 0;
    for (size_t i = 0; i < stacks.size(); i++) {
        if (stacks[i].item && stacks[i].item->GetType().find("chip") != std::string::npos) {
            Chip* chip = static_cast<Chip*>(stacks[i].item);
            totalValue += chip->value * stacks[i].count;
        }
    }
    return totalValue;
}
