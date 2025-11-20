#include "rendering/render_utils.hpp"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>
#include <string>

void DrawTextBillboard(Camera3D camera, const char* text, Vector3 position, float fontSize, Color color) {
    int textWidth = MeasureText(text, (int)fontSize);
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        
        // Calculate rotation to face camera
        Vector3 camPos = camera.position;
        Vector3 objPos = position;
        Vector3 dir = Vector3Subtract(camPos, objPos);
        dir.y = 0; // Keep text upright
        dir = Vector3Normalize(dir);
        
        float angle = atan2f(dir.x, dir.z) * RAD2DEG;
        rlRotatef(angle, 0, 1, 0);
        
        // Scale appropriately for 3D space
        float scale = fontSize * 0.001f;
        rlScalef(scale, -scale, scale);
        
        // Draw the text centered
        DrawText(text, -textWidth/2, 0, (int)fontSize, color);
    rlPopMatrix();
}

void DrawLevelUI(int level, int dimension) {
    // Draw level counter in bottom-right corner
    std::string levelText;
    
    // If in alternate dimension (Salvia), show special text instead of level
    if (dimension > 0) {
        levelText = "SALVIA DIMENSION";
    } else {
        levelText = "LEVEL " + std::to_string(level);
    }
    
    // Get screen dimensions
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calculate position for bottom-right corner
    int textWidth = MeasureText(levelText.c_str(), 30);
    int xPos = screenWidth - textWidth - 25;  // 10px padding inside + 15px text padding
    int yPos = screenHeight - 50;  // 50px from bottom
    
    // Draw with semi-transparent background
    DrawRectangle(xPos - 10, yPos - 5, textWidth + 20, 45, Fade(BLACK, 0.7f));
    
    // Special color for alternate dimensions
    if (dimension > 0) {
        DrawText(levelText.c_str(), xPos, yPos + 5, 30, PURPLE);
    } else {
        DrawText(levelText.c_str(), xPos, yPos + 5, 30, WHITE);
    }
}

void DrawText3D(const char* text, Vector3 position, float fontSize, Color color) {
    int textWidth = MeasureText(text, (int)fontSize);
    
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        
        // Rotate to face forward (away from Z axis)
        rlRotatef(180, 0, 1, 0);  // Face forward
        
        // Much smaller scale for proper size
        float scale = fontSize * 0.0001f;
        rlScalef(scale, -scale, scale);
        
        // Draw the text centered
        DrawText(text, -textWidth/2, 0, (int)fontSize, color);
    rlPopMatrix();
}
