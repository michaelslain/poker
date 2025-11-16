#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <string>

// Difficulty scaling configuration
struct ScalingConfig {
    float insanityMultiplier;    // Multiplier for insanity increase rates
    int minEnemiesPerTable;       // Minimum enemies at poker tables
    int maxEnemiesPerTable;       // Maximum enemies at poker tables
    float resourceSpawnRate;      // Multiplier for chip/weapon/substance spawns (1.0 = normal, 0.5 = half)
    float enemyAIQuality;         // AI thinking time variance (higher = smarter)
    
    // Constructor with default values (level 1 baseline)
    ScalingConfig()
        : insanityMultiplier(1.0f)
        , minEnemiesPerTable(2)
        , maxEnemiesPerTable(3)
        , resourceSpawnRate(1.0f)
        , enemyAIQuality(1.0f)
    {}
};

// Singleton class to manage level progression
class LevelManager {
private:
    static LevelManager* instance;
    
    int currentLevel;              // Current level number (0 = hospital, 1+ = casino levels)
    int currentDimension;          // 0 = normal, 1+ = alternate dimensions (salvia)
    ScalingConfig scaling;         // Current difficulty scaling
    
    // Private constructor for singleton
    LevelManager();
    
    // Calculate scaling based on level number
    void CalculateScaling();

public:
    // Singleton access
    static LevelManager* GetInstance();
    static void Destroy();
    
    // Level progression
    void SetLevel(int level);
    void NextLevel();              // Progress to next level (level + 1)
    void JumpToLevel(int level);   // Jump to specific level (salvia random jump)
    
    // Dimension management
    void EnterAlternateDimension();  // Enter salvia alternate dimension (branch B)
    void ExitAlternateDimension(int levelJump);  // Exit alternate dimension with random jump (option C)
    bool IsInAlternateDimension() const { return currentDimension > 0; }
    
    // Accessors
    int GetCurrentLevel() const { return currentLevel; }
    int GetCurrentDimension() const { return currentDimension; }
    const ScalingConfig& GetScaling() const { return scaling; }
    
    // Generate random level jump for salvia (option C)
    // Weighted toward going up, rarely down, less likely to jump very high
    int GenerateRandomLevelJump() const;
};

#endif // LEVEL_MANAGER_HPP
