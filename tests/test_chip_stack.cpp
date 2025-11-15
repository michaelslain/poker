#include "catch_amalgamated.hpp"
#include <string>

#include "items/chip_stack.hpp"

TEST_CASE("ChipStack - Construction", "[chip_stack]") {
    SECTION("Create empty chip stack") {
        ChipStack stack({0, 0, 0});
        REQUIRE(stack.GetChipCount() == 0);
        REQUIRE(stack.IsEmpty() == true);
        REQUIRE(stack.GetTotalValue() == 0);
    }
    
    SECTION("Create at custom position") {
        ChipStack stack({5, 1, 10});
        REQUIRE(stack.position.x == 5.0f);
        REQUIRE(stack.position.y == 1.0f);
        REQUIRE(stack.position.z == 10.0f);
    }
}

TEST_CASE("ChipStack - GetType", "[chip_stack]") {
    ChipStack stack({0, 0, 0});
    REQUIRE(stack.GetType().find("chip_stack") != std::string::npos);
}

TEST_CASE("ChipStack - AddChip", "[chip_stack]") {
    ChipStack stack({0, 0, 0});
    Chip* chip = new Chip(10, {0, 0, 0}, nullptr);
    
    SECTION("Add single chip") {
        stack.AddChip(chip);
        REQUIRE(stack.GetChipCount() == 1);
        REQUIRE(stack.GetTotalValue() == 10);
        REQUIRE(stack.IsEmpty() == false);
    }
}

TEST_CASE("ChipStack - AddChips", "[chip_stack]") {
    ChipStack stack({0, 0, 0});
    std::vector<Chip*> chips;
    chips.push_back(new Chip(5, {0, 0, 0}, nullptr));
    chips.push_back(new Chip(10, {0, 0, 0}, nullptr));
    chips.push_back(new Chip(25, {0, 0, 0}, nullptr));
    
    stack.AddChips(chips);
    REQUIRE(stack.GetChipCount() == 3);
    REQUIRE(stack.GetTotalValue() == 40); // 5+10+25
}

TEST_CASE("ChipStack - Clear", "[chip_stack]") {
    ChipStack stack({0, 0, 0});
    stack.AddChip(new Chip(10, {0, 0, 0}, nullptr));
    stack.AddChip(new Chip(5, {0, 0, 0}, nullptr));
    
    stack.Clear();
    REQUIRE(stack.GetChipCount() == 0);
    REQUIRE(stack.IsEmpty() == true);
}

TEST_CASE("ChipStack - RemoveAll", "[chip_stack]") {
    ChipStack stack({0, 0, 0});
    stack.AddChip(new Chip(10, {0, 0, 0}, nullptr));
    stack.AddChip(new Chip(25, {0, 0, 0}, nullptr));
    
    std::vector<Chip*> removed = stack.RemoveAll();
    REQUIRE(removed.size() == 2);
    REQUIRE(stack.IsEmpty() == true);
    
    // Cleanup
    for (Chip* chip : removed) {
        delete chip;
    }
}
