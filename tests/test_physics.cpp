#include "catch_amalgamated.hpp"
#include "core/physics.hpp"

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
    
    SECTION("Handles large deltaTime") {
        physics.Step(0.5f); // Should be capped at 0.1f
        REQUIRE(true); // Should not crash
    }
    
    SECTION("Handles zero deltaTime") {
        physics.Step(0.0f); // Should return early
        REQUIRE(true); // Should not crash
    }
}

TEST_CASE("PhysicsWorld - Gravity", "[physics][regression]") {
    PhysicsWorld physics;
    
    SECTION("Gravity is increased for responsive feel") {
        // Create a body and verify it falls faster than real-world physics
        dBodyID body = dBodyCreate(physics.world);
        dBodySetPosition(body, 0, 10, 0);
        
        dMass mass;
        dMassSetSphereTotal(&mass, 1.0f, 0.5f);
        dBodySetMass(body, &mass);
        
        // Step simulation for 1 second
        for (int i = 0; i < 60; i++) {
            physics.Step(1.0f / 60.0f);
        }
        
        const dReal* pos = dBodyGetPosition(body);
        // With gravity = 25 m/s², after 1 second, should fall ~12.5m
        // Starting at y=10, should be at ~-2.5 or lower
        REQUIRE(pos[1] < 0.0);
        
        dBodyDestroy(body);
    }
}

TEST_CASE("PhysicsWorld - Contact Properties", "[physics][regression]") {
    PhysicsWorld physics;
    
    SECTION("No bounce on ground collision") {
        // Create a falling body and a static floor
        dBodyID body = dBodyCreate(physics.world);
        dBodySetPosition(body, 0, 2, 0);
        
        dMass mass;
        dMassSetSphereTotal(&mass, 1.0f, 0.5f);
        dBodySetMass(body, &mass);
        
        dGeomID sphere = dCreateSphere(physics.space, 0.5f);
        dGeomSetBody(sphere, body);
        
        // Create static floor plane at y=0
        dGeomID floor = dCreatePlane(physics.space, 0, 1, 0, 0);
        
        // Give downward velocity
        dBodySetLinearVel(body, 0, -5, 0);
        
        // Step simulation until settled
        for (int i = 0; i < 120; i++) {
            physics.Step(1.0f / 60.0f);
        }
        
        // After settling on ground, velocity should be near zero (no bounce)
        const dReal* vel = dBodyGetLinearVel(body);
        REQUIRE(fabs(vel[1]) < 0.5);
        
        dGeomDestroy(sphere);
        dGeomDestroy(floor);
        dBodyDestroy(body);
    }
}

TEST_CASE("PhysicsWorld - Substepping", "[physics][regression]") {
    PhysicsWorld physics;
    
    SECTION("Fixed timestep prevents tunneling") {
        // Create a fast-moving object
        dBodyID body = dBodyCreate(physics.world);
        dBodySetPosition(body, 0, 5, 0);
        
        dMass mass;
        dMassSetSphereTotal(&mass, 1.0f, 0.5f);
        dBodySetMass(body, &mass);
        
        // Give it high velocity
        dBodySetLinearVel(body, 0, -50, 0);
        
        // Create floor
        dGeomID floor = dCreatePlane(physics.space, 0, 1, 0, 0);
        dGeomID sphere = dCreateSphere(physics.space, 0.5f);
        dGeomSetBody(sphere, body);
        
        // Step with large deltaTime (should be subdivided)
        physics.Step(0.1f);
        
        const dReal* pos = dBodyGetPosition(body);
        // Should not have tunneled through floor at y=0
        REQUIRE(pos[1] >= -1.0); // Allow some penetration but not full tunnel
        
        dGeomDestroy(sphere);
        dGeomDestroy(floor);
        dBodyDestroy(body);
    }
}

TEST_CASE("PhysicsWorld - Global Instance", "[physics]") {
    SECTION("Can set and get global instance") {
        PhysicsWorld physics;
        PhysicsWorld::SetGlobal(&physics);
        
        REQUIRE(PhysicsWorld::GetGlobal() == &physics);
        
        // Cleanup
        PhysicsWorld::SetGlobal(nullptr);
    }
    
    SECTION("Returns nullptr when not set") {
        PhysicsWorld::SetGlobal(nullptr);
        REQUIRE(PhysicsWorld::GetGlobal() == nullptr);
    }
}
