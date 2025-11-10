#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/person.hpp"

// Test concrete Person subclass
class TestPerson : public Person {
public:
    TestPerson(Vector3 pos = {0, 0, 0}, const std::string& name = "Test", float height = 1.0f) 
        : Person(pos, name, height) {}
    
    void Update(float deltaTime) override { (void)deltaTime; }
    std::string GetType() const override { return "test_person"; }
};

TEST_CASE("Person - Construction", "[person]") {
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
    TestPerson person;
    
    SECTION("Set and get body yaw") {
        person.SetBodyYaw(1.57f);
        REQUIRE(person.GetBodyYaw() == Catch::Approx(1.57f));
    }
}

TEST_CASE("Person - Inventory", "[person]") {
    TestPerson person;
    
    SECTION("Has inventory") {
        Inventory* inv = person.GetInventory();
        REQUIRE(inv != nullptr);
        REQUIRE(inv->GetStackCount() == 0);
    }
}

TEST_CASE("Person - PromptBet", "[person]") {
    TestPerson person;
    int raiseAmount = 0;
    
    SECTION("Default implementation returns fold") {
        int action = person.PromptBet(10, 10, 20, 100, raiseAmount);
        REQUIRE(action == 0); // fold
    }
}
