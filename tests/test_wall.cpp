#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/wall.hpp"

TEST_CASE("Wall - Construction", "[wall]") {
    SECTION("Create wall with size") {
        Wall wall({0, 2.5f, 0}, {10, 5, 0.2f}, nullptr);
        REQUIRE(wall.position.x == 0.0f);
        REQUIRE(wall.position.y == 2.5f);
        REQUIRE(wall.position.z == 0.0f);
    }
}

TEST_CASE("Wall - GetType", "[wall]") {
    Wall wall({0, 2.5f, 0}, {10, 5, 0.2f}, nullptr);
    REQUIRE(wall.GetType().find("wall") != std::string::npos);
}

TEST_CASE("Wall - GetGeom", "[wall]") {
    SECTION("GetGeom returns nullptr when physics is null") {
        Wall wall({0, 2.5f, 0}, {10, 5, 0.2f}, nullptr);
        REQUIRE(wall.GetGeom() == nullptr);
    }
    
    SECTION("GetGeom returns valid geom when physics provided") {
        PhysicsWorld physics;
        Wall wall({0, 2.5f, 0}, {10, 5, 0.2f}, &physics);
        REQUIRE(wall.GetGeom() != nullptr);
    }
}
