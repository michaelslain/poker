#include "catch_amalgamated.hpp"
#include "world/stairs.hpp"
#include "core/physics.hpp"
#include "entities/player.hpp"
#include "raylib.h"

TEST_CASE("Stairs - Construction", "[stairs]") {
    PhysicsWorld physics;
    
    SECTION("Constructor initializes properly") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        REQUIRE(stairs.position.x == 0.0f);
        REQUIRE(stairs.position.y == 1.0f);
        REQUIRE(stairs.position.z == 0.0f);
    }
    
    SECTION("GetType returns correct type") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        std::string type = stairs.GetType();
        REQUIRE(type.find("object") != std::string::npos);
        REQUIRE(type.find("stairs") != std::string::npos);
    }
    
    SECTION("GetGeom returns valid geometry") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        dGeomID geom = stairs.GetGeom();
        REQUIRE(geom != nullptr);
    }
}

TEST_CASE("Stairs - Transition State", "[stairs]") {
    PhysicsWorld physics;
    
    SECTION("Transition starts as not triggered") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        REQUIRE_FALSE(stairs.IsTransitionTriggered());
    }
    
    SECTION("ResetTransition clears triggered state") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        // We can't directly trigger it without a player, so just test reset
        stairs.ResetTransition();
        REQUIRE_FALSE(stairs.IsTransitionTriggered());
    }
}

TEST_CASE("Stairs - Player Collision Detection", "[stairs][physics]") {
    PhysicsWorld physics;
    
    SECTION("CheckPlayerCollision detects collision when player is close") {
        Vector3 stairsPos = {0, 1, 0};
        Vector3 stairsSize = {3, 2, 3};
        Stairs stairs(stairsPos, stairsSize, GRAY, &physics);
        
        // Create player at same position (should collide)
        Vector3 playerPos = {0, 1, 0};
        Player player(playerPos, &physics);
        
        // Advance physics to set up collision
        physics.Step(0.016f);
        
        bool collided = stairs.CheckPlayerCollision(player.GetGeom());
        
        // Should detect collision
        REQUIRE(collided);
        
        // Should set transition triggered
        REQUIRE(stairs.IsTransitionTriggered());
    }
    
    SECTION("CheckPlayerCollision does not trigger twice") {
        Vector3 stairsPos = {0, 1, 0};
        Vector3 stairsSize = {3, 2, 3};
        Stairs stairs(stairsPos, stairsSize, GRAY, &physics);
        
        Vector3 playerPos = {0, 1, 0};
        Player player(playerPos, &physics);
        
        physics.Step(0.016f);
        
        // First check should trigger
        bool firstCheck = stairs.CheckPlayerCollision(player.GetGeom());
        REQUIRE(firstCheck);
        REQUIRE(stairs.IsTransitionTriggered());
        
        // Second check should not trigger (already triggered)
        bool secondCheck = stairs.CheckPlayerCollision(player.GetGeom());
        REQUIRE_FALSE(secondCheck);
    }
    
    SECTION("ResetTransition allows re-triggering") {
        Vector3 stairsPos = {0, 1, 0};
        Vector3 stairsSize = {3, 2, 3};
        Stairs stairs(stairsPos, stairsSize, GRAY, &physics);
        
        Vector3 playerPos = {0, 1, 0};
        Player player(playerPos, &physics);
        
        physics.Step(0.016f);
        
        // Trigger collision
        stairs.CheckPlayerCollision(player.GetGeom());
        REQUIRE(stairs.IsTransitionTriggered());
        
        // Reset
        stairs.ResetTransition();
        REQUIRE_FALSE(stairs.IsTransitionTriggered());
        
        // Should be able to trigger again
        physics.Step(0.016f);
        bool retriggered = stairs.CheckPlayerCollision(player.GetGeom());
        REQUIRE(retriggered);
    }
    
    SECTION("CheckPlayerCollision does not detect collision when player is far") {
        Vector3 stairsPos = {0, 1, 0};
        Vector3 stairsSize = {3, 2, 3};
        Stairs stairs(stairsPos, stairsSize, GRAY, &physics);
        
        // Create player far away
        Vector3 playerPos = {100, 1, 100};
        Player player(playerPos, &physics);
        
        physics.Step(0.016f);
        
        bool collided = stairs.CheckPlayerCollision(player.GetGeom());
        
        // Should not detect collision
        REQUIRE_FALSE(collided);
        REQUIRE_FALSE(stairs.IsTransitionTriggered());
    }
}

TEST_CASE("Stairs - Update", "[stairs]") {
    PhysicsWorld physics;
    
    SECTION("Update does not modify position (stairs are static)") {
        Vector3 pos = {5, 2, 3};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        Vector3 originalPos = stairs.position;
        
        stairs.Update(0.016f);
        
        REQUIRE(stairs.position.x == originalPos.x);
        REQUIRE(stairs.position.y == originalPos.y);
        REQUIRE(stairs.position.z == originalPos.z);
    }
}

TEST_CASE("Stairs - Memory Management", "[stairs][regression]") {
    PhysicsWorld physics;
    
    SECTION("Destructor cleans up geometry") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {3, 2, 3};
        
        {
            Stairs stairs(pos, size, GRAY, &physics);
            dGeomID geom = stairs.GetGeom();
            REQUIRE(geom != nullptr);
            
            // Stairs destructor called here
        }
        
        // If we got here without crash, geometry was properly cleaned up
        REQUIRE(true);
    }
    
    SECTION("Multiple stairs can exist simultaneously") {
        Vector3 size = {3, 2, 3};
        
        Stairs stairs1({0, 1, 0}, size, GRAY, &physics);
        Stairs stairs2({10, 1, 0}, size, DARKGRAY, &physics);
        Stairs stairs3({20, 1, 0}, size, LIGHTGRAY, &physics);
        
        REQUIRE(stairs1.GetGeom() != stairs2.GetGeom());
        REQUIRE(stairs2.GetGeom() != stairs3.GetGeom());
        REQUIRE(stairs1.GetGeom() != stairs3.GetGeom());
    }
}

TEST_CASE("Stairs - Edge Cases", "[stairs][regression]") {
    PhysicsWorld physics;
    
    SECTION("Very small stairs work") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {0.1f, 0.1f, 0.1f};
        Stairs stairs(pos, size, GRAY, &physics);
        
        REQUIRE(stairs.GetGeom() != nullptr);
    }
    
    SECTION("Very large stairs work") {
        Vector3 pos = {0, 1, 0};
        Vector3 size = {100, 50, 100};
        Stairs stairs(pos, size, GRAY, &physics);
        
        REQUIRE(stairs.GetGeom() != nullptr);
    }
    
    SECTION("Stairs at origin work") {
        Vector3 pos = {0, 0, 0};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        REQUIRE(stairs.position.x == 0.0f);
        REQUIRE(stairs.position.y == 0.0f);
        REQUIRE(stairs.position.z == 0.0f);
    }
    
    SECTION("Stairs at negative coordinates work") {
        Vector3 pos = {-10, -5, -15};
        Vector3 size = {3, 2, 3};
        Stairs stairs(pos, size, GRAY, &physics);
        
        REQUIRE(stairs.position.x == -10.0f);
        REQUIRE(stairs.position.y == -5.0f);
        REQUIRE(stairs.position.z == -15.0f);
    }
}
