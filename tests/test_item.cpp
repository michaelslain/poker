#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/item.hpp"

TEST_CASE("Item - Construction", "[item]") {
    SECTION("Create at default position") {
        Item item;
        REQUIRE(item.position.x == 0.0f);
        REQUIRE(item.position.y == 0.0f);
        REQUIRE(item.position.z == 0.0f);
        REQUIRE(item.usable == false);  // Items are not usable by default
    }
    
    SECTION("Create at custom position") {
        Item item({5, 10, 15});
        REQUIRE(item.position.x == 5.0f);
        REQUIRE(item.position.y == 10.0f);
        REQUIRE(item.position.z == 15.0f);
        REQUIRE(item.usable == false);  // Items are not usable by default
    }
}

TEST_CASE("Item - GetType", "[item]") {
    Item item;
    REQUIRE(item.GetType().find("item") != std::string::npos);
}

TEST_CASE("Item - Inherits from Interactable", "[item]") {
    Item item;
    
    SECTION("Has interact range") {
        REQUIRE(item.interactRange == 3.0f);
    }
    
    SECTION("Has canInteract flag") {
        REQUIRE(item.canInteract == true);
    }
}
