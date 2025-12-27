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

// Frustum culling: Check if a sphere is within camera view
bool IsInFrustum(const Camera3D& camera, Vector3 position, float radius) {
    // Simple frustum culling using view direction and FOV
    Vector3 toObject = Vector3Subtract(position, camera.position);
    float distance = Vector3Length(toObject);

    // Early out: object behind camera
    Vector3 forward = Vector3Subtract(camera.target, camera.position);
    forward = Vector3Normalize(forward);
    float dot = Vector3DotProduct(toObject, forward);

    if (dot < -radius) return false;  // Behind camera

    // Check angle from view direction (simple cone test)
    if (distance > 0.001f) {  // Avoid division by zero
        float angle = acosf(dot / distance);
        float fovRadians = camera.fovy * DEG2RAD;
        float maxAngle = fovRadians * 0.7f;  // Account for aspect ratio + margin

        return angle < maxAngle + (radius / distance);
    }

    return true;  // Very close to camera, include it
}
