#include "catch_amalgamated.hpp"
#include <string>

#include "entities/enemy.hpp"

TEST_CASE("Enemy - Construction", "[enemy]") {
    SECTION("Create with default name") {
        Enemy enemy({0, 0, 0});
        REQUIRE(enemy.GetName() == "Enemy");
    }
    
    SECTION("Create with custom name") {
        Enemy enemy({0, 0, 0}, "AI Player 1");
        REQUIRE(enemy.GetName() == "AI Player 1");
    }
    
    SECTION("Enemy height is taller (1.5x)") {
        Enemy enemy({0, 0, 0});
        REQUIRE(enemy.GetHeight() == 1.5f);
    }
}

TEST_CASE("Enemy - GetType", "[enemy]") {
    Enemy enemy({0, 0, 0});
    REQUIRE(enemy.GetType().find("enemy") != std::string::npos);
}

TEST_CASE("Enemy - Position", "[enemy]") {
    Enemy enemy({5, 10, 15});
    REQUIRE(enemy.position.x == 5.0f);
    REQUIRE(enemy.position.y == 10.0f);
    REQUIRE(enemy.position.z == 15.0f);
}

TEST_CASE("Enemy - Inventory", "[enemy]") {
    Enemy enemy({0, 0, 0});
    Inventory* inv = enemy.GetInventory();
    REQUIRE(inv != nullptr);
    REQUIRE(inv->GetStackCount() == 0);
}

TEST_CASE("Enemy - Seating", "[enemy]") {
    Enemy enemy({0, 0, 0});
    
    SECTION("Not seated by default") {
        REQUIRE(enemy.IsSeated() == false);
    }
    
    SECTION("Can sit down") {
        enemy.SitDown({5, 1, 0});
        REQUIRE(enemy.IsSeated() == true);
    }
}
