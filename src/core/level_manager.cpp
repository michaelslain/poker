#include "core/level_manager.hpp"
#include "raylib.h"
#include <cmath>
#include <algorithm>

LevelManager* LevelManager::instance = nullptr;

LevelManager::LevelManager()
    : currentLevel(0)
    , currentDimension(0)
{
    CalculateScaling();
}

LevelManager* LevelManager::GetInstance() {
    if (instance == nullptr) {
        instance = new LevelManager();
    }
    return instance;
}

void LevelManager::Destroy() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void LevelManager::CalculateScaling() {
    // Exponential scaling formula: difficulty = 1.0 + (level * 0.2)^1.3
    // This creates a curve where early levels scale slowly, later levels scale faster
    float levelFactor = std::pow(currentLevel * 0.2f, 1.3f);
    
    // Insanity multiplier: 1.0 at level 0, increases exponentially
    scaling.insanityMultiplier = 1.0f + levelFactor;
    
    // Enemies per table: Start at 2-3, cap at 6-7
    scaling.minEnemiesPerTable = std::min(2 + currentLevel / 3, 6);
    scaling.maxEnemiesPerTable = std::min(3 + currentLevel / 2, 7);
    
    // Resource spawn rate: Start at 1.0, decrease to 0.3 at high levels
    scaling.resourceSpawnRate = std::max(1.0f - (currentLevel * 0.05f), 0.3f);
    
    // AI quality: Start at 1.0, increase to 3.0 at high levels (smarter AI)
    scaling.enemyAIQuality = std::min(1.0f + (currentLevel * 0.15f), 3.0f);
}

void LevelManager::SetLevel(int level) {
    currentLevel = level;
    CalculateScaling();
}

void LevelManager::NextLevel() {
    currentLevel++;
    CalculateScaling();
}

void LevelManager::JumpToLevel(int level) {
    currentLevel = level;
    CalculateScaling();
}

void LevelManager::EnterAlternateDimension() {
    currentDimension++;
    // Keep same level number, but in alternate dimension
    // Difficulty could be modified here if alternate dimensions are harder
}

void LevelManager::ExitAlternateDimension(int levelJump) {
    currentDimension = 0;  // Return to normal dimension
    currentLevel += levelJump;  // Apply random jump
    if (currentLevel < 0) {
        currentLevel = 0;  // Can't go below level 0
    }
    CalculateScaling();
}

int LevelManager::GenerateRandomLevelJump() const {
    // Weighted random jump for salvia (option C)
    // 70% chance to go up, 20% chance to stay, 10% chance to go down
    // Jump magnitude: weighted toward small jumps
    
    float directionRoll = GetRandomValue(0, 100) / 100.0f;
    int direction;
    
    if (directionRoll < 0.7f) {
        direction = 1;  // Go up
    } else if (directionRoll < 0.9f) {
        direction = 0;  // Stay same level
    } else {
        direction = -1;  // Go down
    }
    
    if (direction == 0) {
        return 0;
    }
    
    // Magnitude: weighted toward small jumps
    // 50% chance: 1 level
    // 30% chance: 2-3 levels
    // 15% chance: 4-6 levels
    // 5% chance: 7-10 levels
    float magnitudeRoll = GetRandomValue(0, 100) / 100.0f;
    int magnitude;
    
    if (magnitudeRoll < 0.5f) {
        magnitude = 1;
    } else if (magnitudeRoll < 0.8f) {
        magnitude = GetRandomValue(2, 3);
    } else if (magnitudeRoll < 0.95f) {
        magnitude = GetRandomValue(4, 6);
    } else {
        magnitude = GetRandomValue(7, 10);
    }
    
    return direction * magnitude;
}
