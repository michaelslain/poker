#include "catch_amalgamated.hpp"
#include "core/level_manager.hpp"
#include "raylib.h"

TEST_CASE("LevelManager - Singleton", "[level_manager]") {
    // Clean up any existing instance
    LevelManager::Destroy();
    
    SECTION("GetInstance returns valid pointer") {
        LevelManager* manager = LevelManager::GetInstance();
        REQUIRE(manager != nullptr);
    }
    
    SECTION("Multiple GetInstance calls return same instance") {
        LevelManager* manager1 = LevelManager::GetInstance();
        LevelManager* manager2 = LevelManager::GetInstance();
        REQUIRE(manager1 == manager2);
    }
    
    SECTION("Destroy cleans up instance") {
        LevelManager* manager = LevelManager::GetInstance();
        REQUIRE(manager != nullptr);
        
        LevelManager::Destroy();
        
        // After destroy, instance should be cleaned up
        // Getting instance again should create new one
        LevelManager* newManager = LevelManager::GetInstance();
        REQUIRE(newManager != nullptr);
        // Note: Same pointer may be reused due to memory allocation, so we can't test pointer inequality
        // Just verify we can get instance again after destroy
    }
    
    // Cleanup
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Initial State", "[level_manager]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("Starts at level 0") {
        REQUIRE(manager->GetCurrentLevel() == 0);
    }
    
    SECTION("Starts in normal dimension") {
        REQUIRE(manager->GetCurrentDimension() == 0);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
    }
    
    SECTION("Initial scaling is reasonable") {
        const ScalingConfig& scaling = manager->GetScaling();
        REQUIRE(scaling.insanityMultiplier == 1.0f);
        REQUIRE(scaling.minEnemiesPerTable == 2);
        REQUIRE(scaling.maxEnemiesPerTable == 3);
        REQUIRE(scaling.resourceSpawnRate == 1.0f);
        REQUIRE(scaling.enemyAIQuality == 1.0f);
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Level Progression", "[level_manager]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("NextLevel increments level") {
        manager->SetLevel(0);
        REQUIRE(manager->GetCurrentLevel() == 0);
        
        manager->NextLevel();
        REQUIRE(manager->GetCurrentLevel() == 1);
        
        manager->NextLevel();
        REQUIRE(manager->GetCurrentLevel() == 2);
    }
    
    SECTION("SetLevel sets specific level") {
        manager->SetLevel(5);
        REQUIRE(manager->GetCurrentLevel() == 5);
        
        manager->SetLevel(10);
        REQUIRE(manager->GetCurrentLevel() == 10);
        
        manager->SetLevel(0);
        REQUIRE(manager->GetCurrentLevel() == 0);
    }
    
    SECTION("JumpToLevel changes level") {
        manager->SetLevel(3);
        manager->JumpToLevel(7);
        REQUIRE(manager->GetCurrentLevel() == 7);
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Difficulty Scaling", "[level_manager]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("Insanity multiplier increases with level") {
        manager->SetLevel(0);
        float insanity0 = manager->GetScaling().insanityMultiplier;
        
        manager->SetLevel(5);
        float insanity5 = manager->GetScaling().insanityMultiplier;
        
        manager->SetLevel(10);
        float insanity10 = manager->GetScaling().insanityMultiplier;
        
        REQUIRE(insanity5 > insanity0);
        REQUIRE(insanity10 > insanity5);
    }
    
    SECTION("Enemy count increases with level") {
        manager->SetLevel(0);
        int min0 = manager->GetScaling().minEnemiesPerTable;
        int max0 = manager->GetScaling().maxEnemiesPerTable;
        
        manager->SetLevel(10);
        int min10 = manager->GetScaling().minEnemiesPerTable;
        int max10 = manager->GetScaling().maxEnemiesPerTable;
        
        REQUIRE(min10 >= min0);
        REQUIRE(max10 >= max0);
    }
    
    SECTION("Enemy count has reasonable cap") {
        manager->SetLevel(100);  // Very high level
        const ScalingConfig& scaling = manager->GetScaling();
        
        // Should cap at 6-7 enemies per table
        REQUIRE(scaling.minEnemiesPerTable <= 6);
        REQUIRE(scaling.maxEnemiesPerTable <= 7);
    }
    
    SECTION("Resource spawn rate decreases with level") {
        manager->SetLevel(0);
        float resources0 = manager->GetScaling().resourceSpawnRate;
        
        manager->SetLevel(10);
        float resources10 = manager->GetScaling().resourceSpawnRate;
        
        REQUIRE(resources10 < resources0);
    }
    
    SECTION("Resource spawn rate has minimum floor") {
        manager->SetLevel(100);  // Very high level
        const ScalingConfig& scaling = manager->GetScaling();
        
        // Should not go below 0.3
        REQUIRE(scaling.resourceSpawnRate >= 0.3f);
    }
    
    SECTION("AI quality increases with level") {
        manager->SetLevel(0);
        float ai0 = manager->GetScaling().enemyAIQuality;
        
        manager->SetLevel(10);
        float ai10 = manager->GetScaling().enemyAIQuality;
        
        REQUIRE(ai10 > ai0);
    }
    
    SECTION("AI quality has reasonable cap") {
        manager->SetLevel(100);  // Very high level
        const ScalingConfig& scaling = manager->GetScaling();
        
        // Should cap at 3.0
        REQUIRE(scaling.enemyAIQuality <= 3.0f);
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Dimension System", "[level_manager]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("EnterAlternateDimension increments dimension") {
        manager->SetLevel(5);
        REQUIRE(manager->GetCurrentDimension() == 0);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
        
        manager->EnterAlternateDimension();
        REQUIRE(manager->GetCurrentDimension() == 1);
        REQUIRE(manager->IsInAlternateDimension());
        
        // Level should not change
        REQUIRE(manager->GetCurrentLevel() == 5);
    }
    
    SECTION("Multiple dimension entries stack") {
        manager->EnterAlternateDimension();
        REQUIRE(manager->GetCurrentDimension() == 1);
        
        manager->EnterAlternateDimension();
        REQUIRE(manager->GetCurrentDimension() == 2);
    }
    
    SECTION("ExitAlternateDimension returns to normal") {
        manager->SetLevel(5);
        manager->EnterAlternateDimension();
        REQUIRE(manager->IsInAlternateDimension());
        
        manager->ExitAlternateDimension(0);  // No level jump
        REQUIRE(manager->GetCurrentDimension() == 0);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
        REQUIRE(manager->GetCurrentLevel() == 5);
    }
    
    SECTION("ExitAlternateDimension applies level jump") {
        manager->SetLevel(5);
        manager->EnterAlternateDimension();
        
        manager->ExitAlternateDimension(3);  // Jump 3 levels up
        REQUIRE(manager->GetCurrentLevel() == 8);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
    }
    
    SECTION("ExitAlternateDimension with negative jump") {
        manager->SetLevel(10);
        manager->EnterAlternateDimension();
        
        manager->ExitAlternateDimension(-3);  // Jump 3 levels down
        REQUIRE(manager->GetCurrentLevel() == 7);
    }
    
    SECTION("ExitAlternateDimension prevents negative levels") {
        manager->SetLevel(2);
        manager->EnterAlternateDimension();
        
        manager->ExitAlternateDimension(-5);  // Try to go to level -3
        REQUIRE(manager->GetCurrentLevel() == 0);  // Should clamp to 0
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Random Level Jump", "[level_manager]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("GenerateRandomLevelJump always returns positive values") {
        // NEW: Salvia level jump is always forward (+1 to +10)
        for (int i = 0; i < 100; i++) {
            int jump = manager->GenerateRandomLevelJump();
            
            // Jump must be positive (minimum +1, maximum +10)
            REQUIRE(jump >= 1);
            REQUIRE(jump <= 10);
        }
    }
    
    SECTION("GenerateRandomLevelJump covers expected range") {
        // Collect samples to verify we get variety in the 1-10 range
        bool found1 = false;
        bool foundHigh = false;  // >= 7
        
        for (int i = 0; i < 200; i++) {
            int jump = manager->GenerateRandomLevelJump();
            
            if (jump == 1) found1 = true;
            if (jump >= 7) foundHigh = true;
        }
        
        // With 40% chance for +1 and 10% for 7-10, we should see both over 200 samples
        REQUIRE(found1);
        REQUIRE(foundHigh);
    }
    
    SECTION("GenerateRandomLevelJump is non-deterministic") {
        int jump1 = manager->GenerateRandomLevelJump();
        int jump2 = manager->GenerateRandomLevelJump();
        int jump3 = manager->GenerateRandomLevelJump();
        
        // At least one should be different (very high probability)
        bool hasVariety = (jump1 != jump2) || (jump2 != jump3) || (jump1 != jump3);
        REQUIRE(hasVariety);
    }
    
    SECTION("GenerateRandomLevelJump distribution approximation") {
        // Verify rough distribution over many samples
        int countLow = 0;   // 1-3
        int countMid = 0;   // 4-6
        int countHigh = 0;  // 7-10
        
        for (int i = 0; i < 1000; i++) {
            int jump = manager->GenerateRandomLevelJump();
            if (jump <= 3) countLow++;
            else if (jump <= 6) countMid++;
            else countHigh++;
        }
        
        // Expected: ~70% low (1-3), ~20% mid (4-6), ~10% high (7-10)
        // Allow wide margins due to randomness
        REQUIRE(countLow > 500);   // Should be around 700, accept > 500
        REQUIRE(countMid > 100);   // Should be around 200, accept > 100
        REQUIRE(countHigh > 50);   // Should be around 100, accept > 50
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Edge Cases", "[level_manager][regression]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("SetLevel with 0 works") {
        manager->SetLevel(5);
        manager->SetLevel(0);
        REQUIRE(manager->GetCurrentLevel() == 0);
    }
    
    SECTION("SetLevel with very high number works") {
        manager->SetLevel(1000);
        REQUIRE(manager->GetCurrentLevel() == 1000);
        
        // Scaling should still be reasonable (capped values)
        const ScalingConfig& scaling = manager->GetScaling();
        REQUIRE(scaling.minEnemiesPerTable <= 6);
        REQUIRE(scaling.maxEnemiesPerTable <= 7);
        REQUIRE(scaling.resourceSpawnRate >= 0.3f);
        REQUIRE(scaling.enemyAIQuality <= 3.0f);
    }
    
    SECTION("Multiple dimension transitions work") {
        manager->SetLevel(5);
        
        // Enter alternate dimension
        manager->EnterAlternateDimension();
        REQUIRE(manager->GetCurrentDimension() == 1);
        
        // Exit with jump
        manager->ExitAlternateDimension(2);
        REQUIRE(manager->GetCurrentLevel() == 7);
        REQUIRE(manager->GetCurrentDimension() == 0);
        
        // Enter again
        manager->EnterAlternateDimension();
        REQUIRE(manager->GetCurrentDimension() == 1);
        REQUIRE(manager->GetCurrentLevel() == 7);  // Level unchanged
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelManager - Salvia Level Memory", "[level_manager][salvia][regression]") {
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("Remembers level before entering alternate dimension") {
        manager->SetLevel(10);
        manager->EnterAlternateDimension();
        
        // Exit with +5 jump should go to 10 + 5 = 15 (not current level + 5)
        manager->ExitAlternateDimension(5);
        REQUIRE(manager->GetCurrentLevel() == 15);
    }
    
    SECTION("Level memory preserved even if level changes in alternate dimension") {
        manager->SetLevel(7);
        manager->EnterAlternateDimension();
        
        // Simulate level change in alternate dimension (shouldn't happen, but test it)
        manager->SetLevel(20);
        
        // Exit should still go to original level (7) + jump (3) = 10
        manager->ExitAlternateDimension(3);
        REQUIRE(manager->GetCurrentLevel() == 10);
    }
    
    SECTION("Multiple dimension entries update saved level") {
        // First trip from level 5
        manager->SetLevel(5);
        manager->EnterAlternateDimension();
        manager->ExitAlternateDimension(2);  // Should be at 7 now
        REQUIRE(manager->GetCurrentLevel() == 7);
        
        // Second trip from level 7
        manager->EnterAlternateDimension();
        manager->ExitAlternateDimension(3);  // Should be at 7 + 3 = 10
        REQUIRE(manager->GetCurrentLevel() == 10);
    }
    
    SECTION("Zero jump returns to exact pre-trip level") {
        manager->SetLevel(8);
        manager->EnterAlternateDimension();
        manager->ExitAlternateDimension(0);
        REQUIRE(manager->GetCurrentLevel() == 8);
    }
    
    SECTION("Large jump from low level works correctly") {
        manager->SetLevel(0);  // Hospital
        manager->EnterAlternateDimension();
        manager->ExitAlternateDimension(10);  // Maximum Salvia jump
        REQUIRE(manager->GetCurrentLevel() == 10);
    }
    
    SECTION("Negative jump is relative to pre-trip level") {
        manager->SetLevel(15);
        manager->EnterAlternateDimension();
        manager->ExitAlternateDimension(-5);  // Should go to 15 - 5 = 10
        REQUIRE(manager->GetCurrentLevel() == 10);
    }
    
    SECTION("Negative jump with clamping") {
        manager->SetLevel(3);
        manager->EnterAlternateDimension();
        manager->ExitAlternateDimension(-10);  // Should clamp to 0
        REQUIRE(manager->GetCurrentLevel() == 0);
    }
    
    LevelManager::Destroy();
}
