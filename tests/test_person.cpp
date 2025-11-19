#include "catch_amalgamated.hpp"
#include <string>

#include "catch_amalgamated.hpp"
#include "entities/person.hpp"
#include "core/physics.hpp"
#include "core/dom.hpp"

// Test concrete Person subclass
class TestPerson : public Person {
public:
    TestPerson(Vector3 pos = {0, 0, 0}, const std::string& name = "Test", float height = 1.0f) 
        : Person(pos, name, height) {}
    
    std::string GetType() const override { return "test_person"; }
    
    // Expose protected members for testing
    dBodyID GetBody() const { return body; }
    dGeomID GetGeom() const { return geom; }
};

TEST_CASE("Person - Construction", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Create with default height") {
        TestPerson person({0, 0, 0}, "TestPerson");
        REQUIRE(person.GetName() == "TestPerson");
        REQUIRE(person.GetHeight() == 1.0f);
    }
    
    SECTION("Create with custom height") {
        TestPerson person({0, 0, 0}, "Tall", 2.0f);
        REQUIRE(person.GetHeight() == 2.0f);
    }
}

TEST_CASE("Person - Name", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    TestPerson person({0, 0, 0}, "Alice");
    
    SECTION("Get name") {
        REQUIRE(person.GetName() == "Alice");
    }
    
    SECTION("Set name") {
        person.SetName("Bob");
        REQUIRE(person.GetName() == "Bob");
    }
}

TEST_CASE("Person - Height", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    TestPerson person({0, 0, 0}, "Test", 1.5f);
    
    SECTION("Get height") {
        REQUIRE(person.GetHeight() == 1.5f);
    }
    
    SECTION("Set height") {
        person.SetHeight(2.0f);
        REQUIRE(person.GetHeight() == 2.0f);
    }
}

TEST_CASE("Person - Seating", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    TestPerson person;
    
    SECTION("Not seated by default") {
        REQUIRE(person.IsSeated() == false);
    }
    
    SECTION("Can sit down") {
        person.SitDown({5, 1, 0});
        REQUIRE(person.IsSeated() == true);
    }
    
    SECTION("Can stand up") {
        person.SitDown({5, 1, 0});
        person.StandUp();
        REQUIRE(person.IsSeated() == false);
    }
}

TEST_CASE("Person - Body Yaw", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    TestPerson person;
    
    SECTION("Set and get body yaw") {
        person.SetBodyYaw(1.57f);
        REQUIRE(person.GetBodyYaw() == Catch::Approx(1.57f));
    }
}

TEST_CASE("Person - Inventory", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    TestPerson person;
    
    SECTION("Has inventory") {
        Inventory* inv = person.GetInventory();
        REQUIRE(inv != nullptr);
        REQUIRE(inv->GetStackCount() == 0);
    }
}

TEST_CASE("Person - PromptBet", "[person]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    TestPerson person;
    int raiseAmount = 0;
    
    SECTION("Default implementation returns fold") {
        int action = person.PromptBet(10, 10, 20, 100, raiseAmount);
        REQUIRE(action == 0); // fold
    }
}

TEST_CASE("Person - Physics Integration", "[person][physics][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("Person spawns with correct drawing reference height") {
        // NEW SYSTEM: Spawn at feet level Y=0
        TestPerson person({0, 0.0f, 0}, "PhysicsTest");
        
        // Position should match spawn position
        REQUIRE(person.position.y == Catch::Approx(0.0f).margin(0.1f));
    }
    
    SECTION("Person with physics body has correct capsule height") {
        TestPerson person({0, 0.0f, 0}, "Test", 1.0f);  // NEW SYSTEM: feet at Y=0
        
        // For normal height (1.0), capsule height should be 3.4
        // Feet at Y=0.0 (floor level)
        // Body center should be at 0.0 + (3.4/2) = 1.7
        
        if (person.GetBody() != nullptr) {
            const dReal* bodyPos = dBodyGetPosition(person.GetBody());
            REQUIRE(bodyPos[1] == Catch::Approx(1.7f).margin(0.01));
        }
    }
    
    SECTION("Tall person scales capsule correctly") {
        TestPerson tallPerson({0, 0.0f, 0}, "Tall", 2.0f);  // NEW SYSTEM: feet at Y=0
        
        // For height=2.0, capsule height should be 3.4 * 2.0 = 6.8
        // Feet at Y=0.0 (floor)
        // Body center at 0.0 + (6.8/2) = 3.4
        
        if (tallPerson.GetBody() != nullptr) {
            const dReal* bodyPos = dBodyGetPosition(tallPerson.GetBody());
            REQUIRE(bodyPos[1] == Catch::Approx(3.4f).margin(0.01));
        }
    }
    
    SECTION("Person falls with gravity") {
        TestPerson person({0, 5.0f, 0}, "Faller");
        
        // Step physics for a short time
        for (int i = 0; i < 30; i++) {
            physics.Step(1.0f / 60.0f);
            person.Update(1.0f / 60.0f);
        }
        
        // Should have fallen (y position decreased)
        REQUIRE(person.position.y < 5.0f);
    }
    
    SECTION("Person doesn't bounce on ground") {
        TestPerson person({0, 5.0f, 0}, "NoBounce");  // Spawn higher to test landing
        
        // Create floor
        dGeomID floor = dCreatePlane(physics.space, 0, 1, 0, 0);
        
        // Give downward velocity
        if (person.GetBody() != nullptr) {
            dBodySetLinearVel(person.GetBody(), 0, -10, 0);
        }
        
        // Step physics - let it fall and settle
        for (int i = 0; i < 120; i++) {  // More steps to let it fully settle
            physics.Step(1.0f / 60.0f);
            person.Update(1.0f / 60.0f);
        }
        
        // After landing, velocity should be near zero (no bounce)
        if (person.GetBody() != nullptr) {
            const dReal* vel = dBodyGetLinearVel(person.GetBody());
            REQUIRE(fabs(vel[1]) < 0.5);
        }
        
        dGeomDestroy(floor);
    }
    
    // Cleanup
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("Person - Seating Coordinate System", "[person][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("Sitting preserves Y position") {
        TestPerson person({0, 1.3f, 0}, "Sitter");
        float originalY = person.position.y;
        
        // Sit down at a different X/Z but same Y
        person.SitDown({5.0f, originalY, 3.0f});
        
        REQUIRE(person.IsSeated() == true);
        REQUIRE(person.position.x == Catch::Approx(5.0f));
        REQUIRE(person.position.y == Catch::Approx(originalY));
        REQUIRE(person.position.z == Catch::Approx(3.0f));
    }
    
    SECTION("Standing preserves Y position") {
        TestPerson person({0, 1.3f, 0}, "Stander");
        
        // Sit down
        person.SitDown({5.0f, 1.3f, 3.0f});
        float seatedY = person.position.y;
        
        // Stand up
        person.StandUp();
        
        REQUIRE(person.IsSeated() == false);
        // Y should be preserved from seated position
        REQUIRE(person.position.y == Catch::Approx(seatedY));
    }
    
    SECTION("SitDownFacingPoint preserves Y and sets rotation") {
        TestPerson person({0, 1.3f, 0}, "FacingTest");
        float originalY = person.position.y;
        
        // Sit facing a point
        Vector3 seatPos = {5.0f, originalY, 0.0f};
        Vector3 faceTowards = {10.0f, 0.0f, 0.0f};
        
        person.SitDownFacingPoint(seatPos, faceTowards);
        
        REQUIRE(person.IsSeated() == true);
        REQUIRE(person.position.y == Catch::Approx(originalY));
        // Body yaw should be set to face the target
        REQUIRE(person.GetBodyYaw() != 0.0f);
    }
    
    // Cleanup
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}
