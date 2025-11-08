#include "catch_amalgamated.hpp"
#include "../include/item.hpp"

TEST_CASE("Item - Construction", "[item]") {
    SECTION("Create at default position") {
        Item item;
        REQUIRE(item.position.x == 0.0f);
        REQUIRE(item.position.y == 0.0f);
        REQUIRE(item.position.z == 0.0f);
    }
    
    SECTION("Create at custom position") {
        Item item({5, 10, 15});
        REQUIRE(item.position.x == 5.0f);
        REQUIRE(item.position.y == 10.0f);
        REQUIRE(item.position.z == 15.0f);
    }
}

TEST_CASE("Item - GetType", "[item]") {
    Item item;
    REQUIRE(item.GetType() == "item");
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
