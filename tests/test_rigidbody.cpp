#include "catch_amalgamated.hpp"
#include "../include/rigidbody.hpp"
#include "../include/physics.hpp"

TEST_CASE("RigidBody - Construction", "[rigidbody]") {
    SECTION("Create rigid body") {
        RigidBody rb;
        REQUIRE(rb.body == nullptr);
        REQUIRE(rb.geom == nullptr);
        REQUIRE(rb.physics == nullptr);
    }
    
    SECTION("Create at custom position") {
        RigidBody rb({5, 10, 15});
        REQUIRE(rb.position.x == 5.0f);
        REQUIRE(rb.position.y == 10.0f);
        REQUIRE(rb.position.z == 15.0f);
    }
}

TEST_CASE("RigidBody - InitBox", "[rigidbody]") {
    PhysicsWorld physics;
    RigidBody rb;
    
    SECTION("Initialize box") {
        rb.InitBox(&physics, {0, 5, 0}, {1, 1, 1}, 1.0f);
        REQUIRE(rb.body != nullptr);
        REQUIRE(rb.geom != nullptr);
        REQUIRE(rb.physics == &physics);
    }
}

TEST_CASE("RigidBody - InitSphere", "[rigidbody]") {
    PhysicsWorld physics;
    RigidBody rb;
    
    SECTION("Initialize sphere") {
        rb.InitSphere(&physics, {0, 5, 0}, 0.5f, 1.0f);
        REQUIRE(rb.body != nullptr);
        REQUIRE(rb.geom != nullptr);
        REQUIRE(rb.physics == &physics);
    }
}
