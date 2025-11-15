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

    // Configuration constants
    static constexpr float INSANITY_DECREASE_RATE = 0.3f;      // When moving
    static constexpr float INSANITY_INCREASE_SEATED = 0.01f;   // When seated still
    static constexpr float INSANITY_INCREASE_STANDING = 0.02f; // When standing still
    static constexpr float MIN_INSANITY_DECAY_RATE = 0.05f;    // Floor decay rate
    static constexpr float MIN_INSANITY_HOLD_TIME = 30.0f;     // Hold before decay
    static constexpr float KILL_INSANITY_INCREASE = 0.2f;      // Per kill

public:
    InsanityManager(Vector3 startPosition);

    // Update insanity based on player state
    void Update(float deltaTime, Vector3 currentPosition, bool isSeated, bool isTripping, float tripIntensity);

    // Handle kill event
    void OnKill();

    // Getters
    float GetInsanity() const { return insanity; }
    float GetMinInsanity() const { return minInsanity; }

    // Draw the insanity meter UI
    void DrawMeter();
};

#endif // INSANITY_MANAGER_HPP
