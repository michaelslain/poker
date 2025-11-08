#include "catch_amalgamated.hpp"
#include "../include/chip.hpp"

TEST_CASE("Chip - Construction", "[chip]") {
    SECTION("Create chip with value 1") {
        Chip chip(1, {0, 0, 0}, nullptr);
        REQUIRE(chip.value == 1);
        REQUIRE(chip.GetType() == "chip_1");
    }
    
    SECTION("Create chip with value 5") {
        Chip chip(5, {0, 0, 0}, nullptr);
        REQUIRE(chip.value == 5);
        REQUIRE(chip.GetType() == "chip_5");
    }
    
    SECTION("Create chip with value 10") {
        Chip chip(10, {0, 0, 0}, nullptr);
        REQUIRE(chip.value == 10);
        REQUIRE(chip.GetType() == "chip_10");
    }
    
    SECTION("Create chip with value 25") {
        Chip chip(25, {0, 0, 0}, nullptr);
        REQUIRE(chip.value == 25);
        REQUIRE(chip.GetType() == "chip_25");
    }
    
    SECTION("Create chip with value 100") {
        Chip chip(100, {0, 0, 0}, nullptr);
        REQUIRE(chip.value == 100);
        REQUIRE(chip.GetType() == "chip_100");
    }
    
    SECTION("Chip position is set correctly") {
        Vector3 pos = {3.0f, 7.0f, 11.0f};
        Chip chip(5, pos, nullptr);
        
        REQUIRE(chip.position.x == 3.0f);
        REQUIRE(chip.position.y == 7.0f);
        REQUIRE(chip.position.z == 11.0f);
    }
}

TEST_CASE("Chip - GetType", "[chip]") {
    SECTION("GetType returns correct string for all values") {
        REQUIRE(Chip(1, {0,0,0}, nullptr).GetType() == "chip_1");
        REQUIRE(Chip(5, {0,0,0}, nullptr).GetType() == "chip_5");
        REQUIRE(Chip(10, {0,0,0}, nullptr).GetType() == "chip_10");
        REQUIRE(Chip(25, {0,0,0}, nullptr).GetType() == "chip_25");
        REQUIRE(Chip(100, {0,0,0}, nullptr).GetType() == "chip_100");
    }
    
    SECTION("GetType starts with 'chip_'") {
        Chip chip(5, {0,0,0}, nullptr);
        std::string type = chip.GetType();
        REQUIRE(type.substr(0, 5) == "chip_");
    }
}

TEST_CASE("Chip - GetColorFromValue", "[chip]") {
    SECTION("Value 1 returns WHITE") {
        Color color = Chip::GetColorFromValue(1);
        REQUIRE(color.r == WHITE.r);
        REQUIRE(color.g == WHITE.g);
        REQUIRE(color.b == WHITE.b);
    }
    
    SECTION("Value 5 returns RED") {
        Color color = Chip::GetColorFromValue(5);
        REQUIRE(color.r == RED.r);
        REQUIRE(color.g == RED.g);
        REQUIRE(color.b == RED.b);
    }
    
    SECTION("Value 10 returns BLUE") {
        Color color = Chip::GetColorFromValue(10);
        REQUIRE(color.r == BLUE.r);
        REQUIRE(color.g == BLUE.g);
        REQUIRE(color.b == BLUE.b);
    }
    
    SECTION("Value 25 returns GREEN") {
        Color color = Chip::GetColorFromValue(25);
        REQUIRE(color.r == GREEN.r);
        REQUIRE(color.g == GREEN.g);
        REQUIRE(color.b == GREEN.b);
    }
    
    SECTION("Value 100 returns BLACK") {
        Color color = Chip::GetColorFromValue(100);
        REQUIRE(color.r == BLACK.r);
        REQUIRE(color.g == BLACK.g);
        REQUIRE(color.b == BLACK.b);
    }
    
    SECTION("Invalid value throws exception") {
        REQUIRE_THROWS_AS(Chip::GetColorFromValue(999), std::invalid_argument);
        REQUIRE_THROWS_AS(Chip::GetColorFromValue(-1), std::invalid_argument);
        REQUIRE_THROWS_AS(Chip::GetColorFromValue(0), std::invalid_argument);
        REQUIRE_THROWS_AS(Chip::GetColorFromValue(50), std::invalid_argument);
    }
}

TEST_CASE("Chip - Edge Cases", "[chip]") {
    SECTION("Multiple chips with same value are independent") {
        Chip chip1(5, {0, 0, 0}, nullptr);
        Chip chip2(5, {10, 10, 10}, nullptr);
        
        REQUIRE(chip1.value == chip2.value);
        REQUIRE(chip1.GetType() == chip2.GetType());
        REQUIRE(chip1.position.x != chip2.position.x);
        REQUIRE(&chip1 != &chip2);
    }
    
    SECTION("Chip with invalid value 0 throws exception") {
        REQUIRE_THROWS_AS(Chip(0, {0, 0, 0}, nullptr), std::invalid_argument);
    }
    
    SECTION("Chip with negative value throws exception") {
        REQUIRE_THROWS_AS(Chip(-5, {0, 0, 0}, nullptr), std::invalid_argument);
    }
    
    SECTION("Chip with very large value throws exception") {
        REQUIRE_THROWS_AS(Chip(1000000, {0, 0, 0}, nullptr), std::invalid_argument);
    }
}
