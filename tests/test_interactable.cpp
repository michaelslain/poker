#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/interactable.hpp"

TEST_CASE("Interactable - Construction", "[interactable]") {
    SECTION("Create with default interact range") {
        Interactable obj;
        REQUIRE(obj.interactRange == 3.0f);
        REQUIRE(obj.canInteract == true);
    }
    
    SECTION("Create at custom position") {
        Interactable obj({5, 10, 15});
        REQUIRE(obj.position.x == 5.0f);
        REQUIRE(obj.position.y == 10.0f);
        REQUIRE(obj.position.z == 15.0f);
    }
}

TEST_CASE("Interactable - GetType", "[interactable]") {
    Interactable obj;
    REQUIRE(obj.GetType().find("interactable") != std::string::npos);
}

TEST_CASE("Interactable - Callback", "[interactable]") {
    Interactable obj;
    
    SECTION("Can set and call callback") {
        bool called = false;
        obj.onInteract = [&called](Interactable* self) {
            (void)self;
            called = true;
        };
        obj.Interact();
        REQUIRE(called == true);
    }
}

TEST_CASE("Interactable - CanInteract flag", "[interactable]") {
    Interactable obj;
    
    SECTION("Can be disabled") {
        obj.canInteract = false;
        REQUIRE(obj.canInteract == false);
    }
}
