#include "inventory_ui.h"
#include "item.h"
#include "card.h"
#include "chip.h"
#include "raylib.h"

// Static render texture for icon rendering (shared across all icons)
static RenderTexture2D iconRenderTexture = {0};
static bool iconTextureInitialized = false;

void InventoryUI_Draw(Inventory* inventory) {
    // Initialize render texture once
    if (!iconTextureInitialized) {
        iconRenderTexture = LoadRenderTexture(60, 60);
        iconTextureInitialized = true;
    }
    
    // Draw inventory as a horizontal row in the top-left corner
    float iconSize = 60.0f;
    float spacing = 10.0f;
    float startX = 10.0f;
    float startY = 10.0f;
    
    // Isometric-ish camera for rendering icons (angled view)
    Camera3D iconCamera = { 0 };
    iconCamera.position = (Vector3){ 0.8f, 1.2f, 0.8f };  // Angled from top-right
    iconCamera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    iconCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    iconCamera.fovy = 35.0f;
    iconCamera.projection = CAMERA_PERSPECTIVE;
    
    for (int i = 0; i < inventory->stackCount; i++) {
        ItemStack* stack = &inventory->stacks[i];
        Item* item = stack->item;
        
        // Calculate position for this stack
        float x = startX + (iconSize + spacing) * i;
        float y = startY;
        
        Rectangle destRect = { x, y, iconSize, iconSize };
        
        // Render 3D object to texture
        BeginTextureMode(iconRenderTexture);
            ClearBackground((Color){40, 40, 40, 255});
            
            BeginMode3D(iconCamera);
                if (stack->type == ITEM_TYPE_CHIP) {
                    Chip* chip = (Chip*)item;
                    float radius = 0.3f;
                    float height = 0.1f;
                    DrawCylinder((Vector3){0, 0, 0}, radius, radius, height, 16, chip->color);
                    DrawCylinderWires((Vector3){0, 0, 0}, radius, radius, height, 16, DARKGRAY);
                } else {
                    // Card - draw from top
                    float cardWidth = 0.5f;
                    float cardHeight = 0.7f;
                    float cardThickness = 0.02f;
                    
                    DrawCube((Vector3){0, 0, 0}, cardWidth, cardHeight, cardThickness, WHITE);
                    DrawCubeWires((Vector3){0, 0, 0}, cardWidth, cardHeight, cardThickness, DARKGRAY);
                }
            EndMode3D();
        EndTextureMode();
        
        // Draw the texture to screen (flipped vertically)
        Rectangle sourceRec = { 0, 0, (float)iconRenderTexture.texture.width, -(float)iconRenderTexture.texture.height };
        DrawTexturePro(iconRenderTexture.texture, sourceRec, destRect, (Vector2){0, 0}, 0.0f, WHITE);
        
        // Draw border
        DrawRectangleLinesEx(destRect, 2, LIGHTGRAY);
        
        // Draw stack count in bottom-right corner if > 1
        if (stack->count > 1) {
            const char* countText = TextFormat("%d", stack->count);
            int textWidth = MeasureText(countText, 16);
            float textX = x + iconSize - textWidth - 5;
            float textY = y + iconSize - 20;
            
            // Draw text background
            DrawRectangle((int)textX - 2, (int)textY - 2, textWidth + 4, 20, (Color){0, 0, 0, 200});
            
            // Draw white count text
            DrawText(countText, (int)textX, (int)textY, 16, WHITE);
        }
    }
}
