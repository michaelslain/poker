#include "catch_amalgamated.hpp"
#include "../include/physics.hpp"

TEST_CASE("PhysicsWorld - Construction", "[physics]") {
    SECTION("Create physics world") {
        PhysicsWorld physics;
        REQUIRE(physics.world != nullptr);
        REQUIRE(physics.space != nullptr);
        REQUIRE(physics.contactGroup != nullptr);
    }
}

TEST_CASE("PhysicsWorld - Step", "[physics]") {
    PhysicsWorld physics;
    
    SECTION("Can step simulation") {
        physics.Step(0.016f); // 60 FPS
        REQUIRE(true); // If it doesn't crash, it works
    }
}
