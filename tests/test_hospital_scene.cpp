#include "catch_amalgamated.hpp"
#include "scenes/hospital_scene.hpp"
#include "core/dom.hpp"
#include "core/physics.hpp"
#include "raylib.h"

TEST_CASE("HospitalScene - Construction", "[hospital_scene]") {
    PhysicsWorld physics;
    DOM dom;
    
    SECTION("Constructor initializes properly") {
        HospitalScene scene(&physics, &dom);
        
        // Should construct without crashing
        REQUIRE(true);
    }
    
    SECTION("GetPlayerSpawnPosition returns center before generation") {
        HospitalScene scene(&physics, &dom);
        Vector3 spawn = scene.GetPlayerSpawnPosition();
        
        // Should return center of room - NEW SYSTEM: feet at Y=0
        REQUIRE(spawn.x == Catch::Approx(0.0f).margin(0.1f));
        REQUIRE(spawn.y == Catch::Approx(0.0f).margin(0.1f));  // FLOOR_HEIGHT (feet at floor level)
        REQUIRE(spawn.z == Catch::Approx(0.0f).margin(0.1f));
    }
}

TEST_CASE("HospitalScene - Generation", "[hospital_scene]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Generate creates objects in DOM") {
        HospitalScene scene(&physics, &dom);
        int initialCount = dom.GetCount();
        
        scene.Generate();
        
        // Should have created objects (floor, ceiling, walls, stairs, light)
        REQUIRE(dom.GetCount() > initialCount);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Generate creates floor") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
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
    
    SECTION("Generate creates ceiling") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
        bool hasCeiling = false;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("ceiling") != std::string::npos) {
                hasCeiling = true;
                break;
            }
        }
        
        REQUIRE(hasCeiling);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Generate creates walls") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
        int wallCount = 0;
        for (int i = 0; i < dom.GetCount(); i++) {
            std::string type = dom.GetObject(i)->GetType();
            if (type.find("wall") != std::string::npos) {
                wallCount++;
            }
        }
        
        // Should have 4 walls (North, South, East, West)
        REQUIRE(wallCount == 4);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Generate creates stairs") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
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
    
    SECTION("Generate creates light") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
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
}

TEST_CASE("HospitalScene - Spawn Position", "[hospital_scene]") {
    PhysicsWorld physics;
    DOM dom;
    
    SECTION("GetPlayerSpawnPosition returns center after generation") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
        Vector3 spawn = scene.GetPlayerSpawnPosition();
        
        // Should be in center of room - NEW SYSTEM: feet at Y=0
        REQUIRE(spawn.x == Catch::Approx(0.0f).margin(0.1f));
        REQUIRE(spawn.y == Catch::Approx(0.0f).margin(0.1f));  // FLOOR_HEIGHT (feet at floor level)
        REQUIRE(spawn.z == Catch::Approx(0.0f).margin(0.1f));
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
}

TEST_CASE("HospitalScene - Multiple Generations", "[hospital_scene][regression]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Can generate hospital scene multiple times") {
        HospitalScene scene(&physics, &dom);
        
        // First generation
        scene.Generate();
        int count1 = dom.GetCount();
        REQUIRE(count1 > 0);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
        
        // Second generation
        scene.Generate();
        int count2 = dom.GetCount();
        REQUIRE(count2 > 0);
        
        // Should create same number of objects
        REQUIRE(count2 == count1);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
    
    SECTION("Clear does not affect generation") {
        HospitalScene scene(&physics, &dom);
        
        scene.Generate();
        scene.Clear();
        
        // Clear should not remove DOM objects (DOM handles cleanup)
        // But spawn position should still be valid
        Vector3 spawn = scene.GetPlayerSpawnPosition();
        REQUIRE(spawn.y == Catch::Approx(0.0f).margin(0.1f));  // FLOOR_HEIGHT (feet at floor level) - NEW SYSTEM
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
}

TEST_CASE("HospitalScene - Object Count", "[hospital_scene]") {
    PhysicsWorld physics;
    DOM dom;
    DOM::SetGlobal(&dom);
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Hospital scene creates expected number of objects") {
        HospitalScene scene(&physics, &dom);
        scene.Generate();
        
        // Expected objects:
        // 1 floor
        // 1 ceiling
        // 4 walls (North, South, East, West)
        // 1 light
        // 1 stairs
        // 7 debug substances (Weed, Cocaine, Molly, Shrooms, Vodka, Salvia, Fent)
        // Total: 15 objects
        
        REQUIRE(dom.GetCount() == 15);
        
        // Cleanup
        for (int i = 0; i < dom.GetCount(); i++) {
            delete dom.GetObject(i);
        }
        dom.Cleanup();
    }
}
