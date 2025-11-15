#ifndef INSANITY_MANAGER_HPP
#define INSANITY_MANAGER_HPP

#include "raylib.h"

class InsanityManager {
private:
    float insanity;                 // Current insanity level (0.0 to 1.0)
    float minInsanity;              // Minimum insanity floor (from kills)
    float minInsanityDecayTimer;    // Time until min insanity starts decaying
    float timeSinceLastMove;        // Time spent not moving
    Vector3 lastPosition;           // Position from last frame to detect movement

    // Death vignette state
    bool isDying;                   // Death sequence started
    float deathVignetteProgress;    // 0.0 to 1.0 (fully enclosed)
    Shader vignetteShader;          // Vignette post-processing shader
    bool vignetteShaderLoaded;      // Track if shader was loaded successfully

    // Configuration constants
    static constexpr float INSANITY_DECREASE_RATE = 0.3f;      // When moving
    static constexpr float INSANITY_INCREASE_SEATED = 0.01f;   // When seated still
    static constexpr float INSANITY_INCREASE_STANDING = 0.02f; // When standing still
    static constexpr float MIN_INSANITY_DECAY_RATE = 0.05f;    // Floor decay rate
    static constexpr float MIN_INSANITY_HOLD_TIME = 30.0f;     // Hold before decay
    static constexpr float KILL_INSANITY_INCREASE = 0.2f;      // Per kill
    static constexpr float DEATH_VIGNETTE_DURATION = 3.0f;     // 3 seconds to close

public:
    InsanityManager(Vector3 startPosition);
    ~InsanityManager();

    // Update insanity based on player state
    void Update(float deltaTime, Vector3 currentPosition, bool isSeated, bool isTripping, float tripIntensity);

    // Handle kill event
    void OnKill();

    // Getters
    float GetInsanity() const { return insanity; }
    float GetMinInsanity() const { return minInsanity; }
    bool IsDying() const { return isDying; }
    bool IsDeathComplete() const { return isDying && deathVignetteProgress >= 1.0f; }

    // Draw the insanity meter UI
    void DrawMeter();
    
    // Draw death vignette overlay (draws full-screen vignette using shader)
    void DrawDeathVignette();
};

#endif // INSANITY_MANAGER_HPP
