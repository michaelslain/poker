#include "catch_amalgamated.hpp"
#include <string>

#include "world/wall.hpp"

TEST_CASE("Wall - Construction", "[wall]") {
    SECTION("Create wall with size") {
        Wall wall({0, 2.5f, 0}, {10, 5, 0.2f});
        REQUIRE(wall.position.x == 0.0f);
        REQUIRE(wall.position.y == 2.5f);
        REQUIRE(wall.position.z == 0.0f);
    }
}

TEST_CASE("Wall - GetType", "[wall]") {
    Wall wall({0, 2.5f, 0}, {10, 5, 0.2f});
    REQUIRE(wall.GetType().find("wall") != std::string::npos);
}

// NOTE: Wall now uses global physics from PhysicsWorld::GetGlobal()
// Collision geometry is internal and not exposed via GetCollider()
TEST_CASE("Wall - GetGeom", "[wall]") {
    SECTION("Wall creates successfully") {
        Wall wall({0, 2.5f, 0}, {10, 5, 0.2f});
        REQUIRE(wall.GetType().find("wall") != std::string::npos);
    }
}
