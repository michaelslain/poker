#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/floor.hpp"

TEST_CASE("Floor - Construction", "[floor]") {
    SECTION("Create floor with size and color") {
        Floor floor({0, 0, 0}, {10, 10}, GRAY, nullptr);
        REQUIRE(floor.position.x == 0.0f);
        REQUIRE(floor.position.y == 0.0f);
        REQUIRE(floor.position.z == 0.0f);
    }
}

TEST_CASE("Floor - GetType", "[floor]") {
    Floor floor({0, 0, 0}, {10, 10}, GRAY, nullptr);
    REQUIRE(floor.GetType().find("floor") != std::string::npos);
}
