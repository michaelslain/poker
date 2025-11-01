#include "inventory_ui.h"
#include "item.h"
#include "card.h"
#include "raylib.h"

void InventoryUI_Draw(Inventory* inventory) {
    // Draw inventory as a horizontal row in the top-left corner
    float iconSize = 60.0f;
    float spacing = 10.0f;
    float startX = 10.0f;
    float startY = 10.0f;
    
    for (int i = 0; i < inventory->count; i++) {
        Item* item = inventory->items[i];
        
        // Calculate position for this item
        float x = startX + (iconSize + spacing) * i;
        float y = startY;
        
        Rectangle destRect = { x, y, iconSize, iconSize };
        
        // Check if this is a Card and use its DrawIcon if so
        // Note: This is a simple type check - in a real system you might want a vtable
        Card* card = (Card*)item;
        if (card->textureLoaded) {
            Card_DrawIcon(card, destRect);
        } else {
            // Fallback to default item icon
            Item_DrawIcon(item, destRect);
        }
    }
}
