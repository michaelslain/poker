#include "chip_stack.hpp"
#include <algorithm>

ChipStack::ChipStack(Vector3 pos) : Object(pos) {
}

ChipStack::~ChipStack() {
    // Note: We don't delete chips here - they're managed elsewhere
    // Just clear our references
    chips.clear();
    chipsByValue.clear();
}

void ChipStack::Update(float deltaTime) {
    (void)deltaTime;
    
    // Update all chips
    for (Chip* chip : chips) {
        if (chip) {
            chip->Update(deltaTime);
        }
    }
}

void ChipStack::Draw(Camera3D camera) {
    // Draw all chips manually (they're not in DOM)
    for (Chip* chip : chips) {
        if (chip) {
            chip->Draw(camera);
        }
    }
}

std::string ChipStack::GetType() const {
    return "chip_stack";
}

void ChipStack::AddChip(Chip* chip) {
    if (!chip) return;
    
    chips.push_back(chip);
    chipsByValue[chip->value].push_back(chip);
    
    // Reorganize positions
    OrganizeChips();
}

void ChipStack::AddChips(const std::vector<Chip*>& newChips) {
    for (Chip* chip : newChips) {
        if (chip) {
            chips.push_back(chip);
            chipsByValue[chip->value].push_back(chip);
        }
    }
    
    // Reorganize positions
    OrganizeChips();
}

void ChipStack::Clear() {
    chips.clear();
    chipsByValue.clear();
}

std::vector<Chip*> ChipStack::RemoveAll() {
    std::vector<Chip*> result = chips;
    chips.clear();
    chipsByValue.clear();
    return result;
}

int ChipStack::GetTotalValue() const {
    int total = 0;
    for (Chip* chip : chips) {
        if (chip) {
            total += chip->value;
        }
    }
    return total;
}

void ChipStack::OrganizeChips() {
    // Stack chips by denomination in separate piles
    // Denominations: 100 (BLACK), 25 (GREEN), 10 (BLUE), 5 (RED), 1 (WHITE)
    
    float chipHeight = 0.03f;  // Height of one chip
    float pileSpacing = 0.25f;  // Space between different denomination piles
    
    // Sort denominations in descending order
    std::vector<int> denoms = {100, 25, 10, 5, 1};
    
    float currentX = position.x;
    
    for (int denom : denoms) {
        if (chipsByValue.find(denom) == chipsByValue.end()) continue;
        if (chipsByValue[denom].empty()) continue;
        
        std::vector<Chip*>& pile = chipsByValue[denom];
        
        // Stack chips vertically
        for (size_t i = 0; i < pile.size(); i++) {
            Chip* chip = pile[i];
            if (chip) {
                chip->position = {
                    currentX,
                    position.y + (i * chipHeight),
                    position.z
                };
                chip->rotation = {0, 0, 0};  // Chips lie flat
            }
        }
        
        // Move to next pile position
        currentX += pileSpacing;
    }
}
