#include "gameplay/insanity_manager.hpp"
#include "raymath.h"

InsanityManager::InsanityManager(Vector3 startPosition)
    : insanity(0.0f), minInsanity(0.0f), minInsanityDecayTimer(0.0f),
      timeSinceLastMove(0.0f), lastPosition(startPosition),
      isDying(false), deathVignetteProgress(0.0f), vignetteShaderLoaded(false)
{
    // Load vignette shader (optional - game still works if it fails)
    TraceLog(LOG_INFO, "INSANITY: Attempting to load vignette shader...");
    vignetteShader = LoadShader("shaders/vignette.vs", "shaders/vignette.fs");
    if (vignetteShader.id != 0) {
        vignetteShaderLoaded = true;
        TraceLog(LOG_INFO, "INSANITY: Vignette shader loaded successfully (ID: %d)", vignetteShader.id);
    } else {
        TraceLog(LOG_WARNING, "INSANITY: Failed to load vignette shader - death effect will be disabled");
    }
    TraceLog(LOG_INFO, "INSANITY: InsanityManager initialized");
}

InsanityManager::~InsanityManager() {
    if (vignetteShaderLoaded) {
        UnloadShader(vignetteShader);
    }
}

void InsanityManager::Update(float deltaTime, Vector3 currentPosition, bool isSeated, bool isTripping, float tripIntensity) {
    // Handle death vignette progression
    if (isDying) {
        deathVignetteProgress += deltaTime / DEATH_VIGNETTE_DURATION;
        if (deathVignetteProgress > 1.0f) deathVignetteProgress = 1.0f;
        return; // Don't update insanity while dying
    }
    
    // Check if insanity reached maximum - trigger death
    if (insanity >= 1.0f && !isDying) {
        isDying = true;
        deathVignetteProgress = 0.0f;
    }
    
    // If tripping, insanity is trip intensity + minimum floor
    if (isTripping) {
        insanity = tripIntensity + minInsanity;
        if (insanity > 1.0f) insanity = 1.0f;
    } else {
        // Normal insanity system - detect if player is moving
        float distanceMoved = Vector3Distance(currentPosition, lastPosition);
        bool isMoving = distanceMoved > 0.01f;  // Threshold to detect movement

        if (isMoving) {
            // Player is moving - decrease insanity
            insanity -= deltaTime * INSANITY_DECREASE_RATE;
            timeSinceLastMove = 0.0f;
        } else {
            // Player is standing still - increase insanity
            timeSinceLastMove += deltaTime;
            
            // Different rates for seated vs standing
            float insanityIncreaseRate = isSeated ? INSANITY_INCREASE_SEATED : INSANITY_INCREASE_STANDING;
            insanity += deltaTime * insanityIncreaseRate;
        }

        // Handle minimum insanity floor decay
        if (minInsanityDecayTimer > 0.0f) {
            minInsanityDecayTimer -= deltaTime;
        } else if (minInsanity > 0.0f) {
            // After timer expires, slowly decay the minimum insanity floor
            minInsanity -= deltaTime * MIN_INSANITY_DECAY_RATE;
            if (minInsanity < 0.0f) minInsanity = 0.0f;
        }

        // Clamp insanity between minInsanity and 1.0
        if (insanity < minInsanity) insanity = minInsanity;
        if (insanity > 1.0f) insanity = 1.0f;
    }

    // Update last position for next frame
    lastPosition = currentPosition;
}

void InsanityManager::OnKill() {
    // Increase minimum insanity floor (stacks with multiple kills)
    minInsanity += KILL_INSANITY_INCREASE;
    if (minInsanity > 1.0f) minInsanity = 1.0f;
    
    // Set timer before the floor starts decaying
    minInsanityDecayTimer = MIN_INSANITY_HOLD_TIME;
    
    // Immediately set insanity to at least the new minimum
    if (insanity < minInsanity) {
        insanity = minInsanity;
    }
}

void InsanityManager::DrawMeter() {
    // Draw N64-style circular power meter in top-right corner
    int screenWidth = GetScreenWidth();

    // Position in top-right corner
    float centerX = screenWidth - 80.0f;
    float centerY = 80.0f;
    float radius = 50.0f;

    // Draw outer circle (border)
    DrawCircle((int)centerX, (int)centerY, radius + 3.0f, BLACK);
    DrawCircle((int)centerX, (int)centerY, radius, DARKGRAY);

    // Draw filled arc representing insanity level (like N64 power meter)
    // The arc fills clockwise from top
    int segments = 32;
    float startAngle = -90.0f;  // Start at top
    float endAngle = startAngle + (insanity * 360.0f);  // Fill based on insanity

    // Draw the insanity fill as a pie slice
    if (insanity > 0.0f) {
        // Color shifts from yellow to red as insanity increases
        Color fillColor;
        if (insanity < 0.5f) {
            // Yellow to orange (0.0 - 0.5)
            fillColor = (Color){255, (unsigned char)(255 - (insanity * 2 * 100)), 0, 200};
        } else {
            // Orange to red (0.5 - 1.0)
            fillColor = (Color){255, (unsigned char)(155 - ((insanity - 0.5f) * 2 * 155)), 0, 200};
        }

        DrawCircleSector((Vector2){centerX, centerY}, radius - 5.0f,
                        startAngle, endAngle, segments, fillColor);
    }

    // Draw inner circle (center)
    DrawCircle((int)centerX, (int)centerY, radius - 10.0f, BLACK);

    // Draw "INSANITY" text in center
    const char* text = "INSANITY";
    int fontSize = 10;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (int)(centerX - textWidth / 2.0f), (int)(centerY - fontSize / 2.0f),
             fontSize, WHITE);
}

void InsanityManager::DrawDeathVignette() {
    if (!isDying || !vignetteShaderLoaded) return;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Set shader uniform for vignette progress
    int progressLoc = GetShaderLocation(vignetteShader, "progress");
    SetShaderValue(vignetteShader, progressLoc, &deathVignetteProgress, SHADER_UNIFORM_FLOAT);
    
    // Enable alpha blending for overlay effect
    BeginBlendMode(BLEND_ALPHA);
    BeginShaderMode(vignetteShader);
    
    // Draw full-screen quad - shader outputs black with varying alpha
    DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
    
    EndShaderMode();
    EndBlendMode();
}
