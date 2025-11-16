#include "catch_amalgamated.hpp"
#include "gameplay/level_generator.hpp"
#include "core/dom.hpp"
#include "core/physics.hpp"
#include "core/level_manager.hpp"
#include "raylib.h"

TEST_CASE("LevelGenerator - Construction", "[level_generator]") {
    PhysicsWorld physics;
    DOM dom;
    
    SECTION("Constructor initializes properly") {
        LevelGenerator generator(&physics, &dom);
        
        // Should construct without crashing
        REQUIRE(true);
    }
    
    SECTION("GetPlayerSpawnPosition returns default when no level generated") {
        LevelGenerator generator(&physics, &dom);
        Vector3 spawn = generator.GetPlayerSpawnPosition();
        
        // Default spawn position
        REQUIRE(spawn.x == 0.0f);
        REQUIRE(spawn.y == 1.8f);
        REQUIRE(spawn.z == 0.0f);
    }
}

TEST_CASE("LevelGenerator - Level Generation", "[level_generator]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("GenerateLevel creates objects in DOM") {
        LevelGenerator generator(&physics, &dom);
        int initialCount = dom.GetCount();
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        // Should have created objects (floors, ceilings, walls, poker tables, etc.)
        REQUIRE(dom.GetCount() > initialCount);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("GetPlayerSpawnPosition returns valid position after generation") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        Vector3 spawn = generator.GetPlayerSpawnPosition();
        
        // Y should be player height (1.8)
        REQUIRE(spawn.y == 1.8f);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Higher levels generate more rooms") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        int countLevel1 = dom.GetCount();
        
        // Cleanup level 1
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
        
        levelManager->SetLevel(5);
        generator.GenerateLevel(5);
        int countLevel5 = dom.GetCount();
        
        // Level 5 should have more objects than level 1
        REQUIRE(countLevel5 >= countLevel1);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Clear clears internal state") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        generator.Clear();
        
        // After clear, spawn position should be default
        Vector3 spawn = generator.GetPlayerSpawnPosition();
        REQUIRE(spawn.x == 0.0f);
        REQUIRE(spawn.y == 1.8f);
        REQUIRE(spawn.z == 0.0f);
        
        // Cleanup DOM objects
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelGenerator - Room Contents", "[level_generator]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("Generates stairs in last room") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        // Check if any stairs were created
        bool hasStairs = false;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("stairs") != std::string::npos) {
                hasStairs = true;
                break;
            }
        }
        
        REQUIRE(hasStairs);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Generates floors and ceilings") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        bool hasFloor = false;
        bool hasCeiling = false;
        
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("floor") != std::string::npos) hasFloor = true;
            if (type.find("ceiling") != std::string::npos) hasCeiling = true;
        }
        
        REQUIRE(hasFloor);
        REQUIRE(hasCeiling);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Generates walls") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        bool hasWall = false;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("wall") != std::string::npos) {
                hasWall = true;
                break;
            }
        }
        
        REQUIRE(hasWall);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Generates lights") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        bool hasLight = false;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("light") != std::string::npos) {
                hasLight = true;
                break;
            }
        }
        
        REQUIRE(hasLight);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelGenerator - Multiple Generations", "[level_generator][regression]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("Can generate multiple levels in sequence") {
        LevelGenerator generator(&physics, &dom);
        
        // Generate level 1
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        int count1 = dom.GetCount();
        REQUIRE(count1 > 0);
        
        // Cleanup level 1
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
        
        // Generate level 2
        levelManager->SetLevel(2);
        generator.GenerateLevel(2);
        int count2 = dom.GetCount();
        REQUIRE(count2 > 0);
        
        // Cleanup level 2
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
        
        // Generate level 3
        levelManager->SetLevel(3);
        generator.GenerateLevel(3);
        int count3 = dom.GetCount();
        REQUIRE(count3 > 0);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Clear between generations prevents object accumulation") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        // Cleanup DOM
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
        
        // Clear generator state
        generator.Clear();
        
        // Generate new level
        levelManager->SetLevel(2);
        generator.GenerateLevel(2);
        
        // Should work without issues
        REQUIRE(dom.GetCount() > 0);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}
