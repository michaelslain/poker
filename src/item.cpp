#include "item.hpp"

Item::Item(Vector3 pos)
    : Interactable(pos)
{
}

void Item::DrawIcon(Rectangle destRect) {
    // Default icon - just a gray square
    DrawRectangleRec(destRect, GRAY);
    DrawRectangleLinesEx(destRect, 2, DARKGRAY);
}

std::string Item::GetType() const {
    return Interactable::GetType() + "_item";
}

const char* Item::GetDisplayCount(int stackCount) const {
    if (stackCount > 1) {
        return TextFormat("%d", stackCount);
    }
    return nullptr;  // No count display for single items
}
