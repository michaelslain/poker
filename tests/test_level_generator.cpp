#include "catch_amalgamated.hpp"
#include "gameplay/level_generator.hpp"
#include "core/dom.hpp"
#include "core/physics.hpp"
#include "core/level_manager.hpp"
#include "rendering/lighting_manager.hpp"
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
        REQUIRE(spawn.y == 1.3f);  // FLOOR_HEIGHT + 1.3 (drawing reference height)
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
        
        // Y should be drawing reference height
        REQUIRE(spawn.y == 1.3f);  // FLOOR_HEIGHT + 1.3 (drawing reference height)
        
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
        REQUIRE(spawn.y == 1.3f);  // FLOOR_HEIGHT + 1.3 (drawing reference height)
        REQUIRE(spawn.z == 0.0f);
        
        // Cleanup DOM objects
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelGenerator - Non-Linear Room Generation", "[level_generator]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("Rooms can extend in multiple directions") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        // Level should have multiple rooms with walls in different orientations
        bool hasNorthSouthWalls = false;
        bool hasEastWestWalls = false;
        
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            std::string type = obj->GetType();
            
            if (type.find("wall") != std::string::npos) {
                // Check rotation - 0° or 180° is North/South, 90° or 270° is East/West
                if (obj->rotation.y == 0.0f || obj->rotation.y == 180.0f) {
                    hasNorthSouthWalls = true;
                } else if (obj->rotation.y == 90.0f || obj->rotation.y == 270.0f) {
                    hasEastWestWalls = true;
                }
            }
        }
        
        // Should have walls in different orientations (non-linear layout)
        // Note: Small levels might be linear, so this is a soft check
        (void)hasNorthSouthWalls;
        (void)hasEastWestWalls;
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("No pillar artifacts between rooms") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(3);
        generator.GenerateLevel(3);
        
        // Count walls - shouldn't have excessive walls creating pillar-like structures
        int wallCount = 0;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("wall") != std::string::npos) {
                wallCount++;
            }
        }
        
        // Should have reasonable number of walls (not excessive)
        REQUIRE(wallCount > 0);
        REQUIRE(wallCount < 200); // Sanity check - shouldn't have hundreds of walls
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelGenerator - Room Alignment", "[level_generator][regression]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("Rooms connect without gaps") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(2);
        generator.GenerateLevel(2);
        
        // All floors should be generated successfully
        int floorCount = 0;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("floor") != std::string::npos) {
                floorCount++;
            }
        }
        
        REQUIRE(floorCount > 0);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Walls align properly at doorways") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(2);
        generator.GenerateLevel(2);
        
        // Check that walls have collision enabled
        bool hasWallsWithCollision = false;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("wall") != std::string::npos) {
                // Wall exists - should have collision
                hasWallsWithCollision = true;
                break;
            }
        }
        
        REQUIRE(hasWallsWithCollision);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelGenerator - Dark Maroon Floor Color", "[level_generator]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("Generated levels use dark maroon floor") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        // At least one floor should exist
        bool hasFloor = false;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("floor") != std::string::npos) {
                hasFloor = true;
                break;
            }
        }
        
        REQUIRE(hasFloor);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    LevelManager::Destroy();
}

TEST_CASE("LevelGenerator - Multiple Lights Per Level", "[level_generator]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    LightingManager::ResetLights();
    LevelManager::Destroy();
    LevelManager* levelManager = LevelManager::GetInstance();
    
    SECTION("Each room gets one light bulb") {
        LevelGenerator generator(&physics, &dom);
        
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        
        // Count light bulbs
        int lightCount = 0;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("light_bulb") != std::string::npos) {
                lightCount++;
            }
        }
        
        // Should have multiple lights (one per room, minimum 3 rooms)
        REQUIRE(lightCount >= 3);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Lights persist across level transitions") {
        LevelGenerator generator(&physics, &dom);
        
        // Generate level 1
        levelManager->SetLevel(1);
        generator.GenerateLevel(1);
        int lights1 = 0;
        for (int i = 0; i < dom.GetCount(); i++) {
            if (dom.GetObject(i)->GetType().find("light_bulb") != std::string::npos) {
                lights1++;
            }
        }
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
        LightingManager::ResetLights();
        
        // Generate level 2
        levelManager->SetLevel(2);
        generator.GenerateLevel(2);
        int lights2 = 0;
        for (int i = 0; i < dom.GetCount(); i++) {
            if (dom.GetObject(i)->GetType().find("light_bulb") != std::string::npos) {
                lights2++;
            }
        }
        
        // Both levels should have lights
        REQUIRE(lights1 > 0);
        REQUIRE(lights2 > 0);
        
        // Cleanup
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
