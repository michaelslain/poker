#include "inventory_ui.hpp"
#include "inventory.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "pistol.hpp"
#include "raylib.h"
#include <cstring>

void InventoryUI_Draw(Inventory* inventory, int selectedIndex) {
    // Draw inventory as a horizontal row in the top-left corner
    float iconSize = 60.0f;
    float spacing = 10.0f;
    float startX = 10.0f;
    float startY = 10.0f;
    
    for (int i = 0; i < inventory->GetStackCount(); i++) {
        ItemStack* stack = inventory->GetStack(i);
        Item* item = stack->item;
        
        // Skip if item pointer is null
        if (item == nullptr) continue;
        
        // Calculate position for this stack
        float x = startX + (iconSize + spacing) * i;
        float y = startY;
        
        Rectangle destRect = { x, y, iconSize, iconSize };
        
        // Use the item's DrawIcon method
        item->DrawIcon(destRect);
        
        // Draw border - white if selected, light gray otherwise
        if (i == selectedIndex) {
            DrawRectangleLinesEx(destRect, 3, WHITE);  // Thicker white outline for selected
        } else {
            DrawRectangleLinesEx(destRect, 2, LIGHTGRAY);
        }
        
        // Draw count in bottom-right corner (uses polymorphic GetDisplayCount)
        const char* countText = item->GetDisplayCount(stack->count);
        
        if (countText != nullptr) {
            int textWidth = MeasureText(countText, 16);
            float textX = x + iconSize - textWidth - 5;
            float textY = y + iconSize - 20;
            
            // Draw text background
            DrawRectangle((int)textX - 2, (int)textY - 2, textWidth + 4, 20, {0, 0, 0, 200});
            
            // Draw white count text
            DrawText(countText, (int)textX, (int)textY, 16, WHITE);
        }
    }
}
