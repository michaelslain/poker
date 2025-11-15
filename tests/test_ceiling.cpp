#include "catch_amalgamated.hpp"
#include <string>

#include "world/ceiling.hpp"

TEST_CASE("Ceiling - Construction", "[ceiling]") {
    SECTION("Create ceiling with size and color") {
        Ceiling ceiling({0, 5, 0}, {10, 10}, WHITE, nullptr);
        REQUIRE(ceiling.position.x == 0.0f);
        REQUIRE(ceiling.position.y == 5.0f);
        REQUIRE(ceiling.position.z == 0.0f);
    }
}

TEST_CASE("Ceiling - GetType", "[ceiling]") {
    Ceiling ceiling({0, 5, 0}, {10, 10}, WHITE, nullptr);
    REQUIRE(ceiling.GetType().find("ceiling") != std::string::npos);
}
