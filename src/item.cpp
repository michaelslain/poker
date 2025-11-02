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

const char* Item::GetType() const {
    return "item";
}
