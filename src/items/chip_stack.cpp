#include "items/chip_stack.hpp"
#include "core/dom.hpp"
#include "raylib.h"

ChipStack::ChipStack(Vector3 pos) : Object(pos) {
}

ChipStack::~ChipStack() {
    // Remove chips from DOM and delete them
    DOM* dom = DOM::GetGlobal();
    if (dom) {
        for (Chip* chip : chips) {
            if (chip) {
                dom->RemoveObject(chip);
                delete chip;
            }
        }
    }
    chips.clear();
    chipsByValue.clear();
}

void ChipStack::Update(float deltaTime) {
    (void)deltaTime;
    // Chips update themselves via DOM - no need to update them here
}

void ChipStack::Draw(Camera3D camera) {
    (void)camera;
    // Chips render themselves via DOM - no need to draw them here
}

std::string ChipStack::GetType() const {
    return Object::GetType() + "_chip_stack";
}

void ChipStack::AddChip(Chip* chip) {
    if (!chip) return;

    chips.push_back(chip);
    chipsByValue[chip->value].push_back(chip);

    // Make chip non-interactable (it's in the pot)
    chip->canInteract = false;

    // Add chip to DOM so it renders
    DOM* dom = DOM::GetGlobal();
    if (dom) {
        dom->AddObject(chip);
    }

    // Reorganize positions
    OrganizeChips();
}

void ChipStack::AddChips(const std::vector<Chip*>& newChips) {
    DOM* dom = DOM::GetGlobal();

    for (Chip* chip : newChips) {
        if (chip) {
            chips.push_back(chip);
            chipsByValue[chip->value].push_back(chip);

            // Make chip non-interactable (it's in the pot)
            chip->canInteract = false;

            // Add chip to DOM so it renders
            if (dom) {
                dom->AddObject(chip);
            }
        }
    }

    // Reorganize positions
    OrganizeChips();
}

void ChipStack::Clear() {
    // Remove chips from DOM (but don't delete them - caller manages that)
    DOM* dom = DOM::GetGlobal();
    if (dom) {
        for (Chip* chip : chips) {
            if (chip) {
                dom->RemoveObject(chip);
            }
        }
    }

    chips.clear();
    chipsByValue.clear();
}

std::vector<Chip*> ChipStack::RemoveAll() {
    // Remove chips from DOM (caller will manage deletion)
    DOM* dom = DOM::GetGlobal();
    if (dom) {
        for (Chip* chip : chips) {
            if (chip) {
                dom->RemoveObject(chip);
            }
        }
    }

    std::vector<Chip*> result = chips;
    chips.clear();
    chipsByValue.clear();
    return result;
}

void ChipStack::MakeAllInteractable() {
    for (Chip* chip : chips) {
        if (chip) {
            chip->canInteract = true;
        }
    }
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
