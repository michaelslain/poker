#include "catch_amalgamated.hpp"
#include "../include/dealer.hpp"

TEST_CASE("Dealer - Construction", "[dealer]") {
    SECTION("Create with default name") {
        Dealer dealer({0, 0, 0});
        REQUIRE(dealer.GetName() == "Dealer");
    }
    
    SECTION("Create with custom name") {
        Dealer dealer({0, 0, 0}, "The Dealer");
        REQUIRE(dealer.GetName() == "The Dealer");
    }
    
    SECTION("Dealer has normal height") {
        Dealer dealer({0, 0, 0});
        REQUIRE(dealer.GetHeight() == 1.0f);
    }
}

TEST_CASE("Dealer - GetType", "[dealer]") {
    Dealer dealer({0, 0, 0});
    REQUIRE(dealer.GetType() == "dealer");
}

TEST_CASE("Dealer - Position", "[dealer]") {
    Dealer dealer({10, 2, 5});
    REQUIRE(dealer.position.x == 10.0f);
    REQUIRE(dealer.position.y == 2.0f);
    REQUIRE(dealer.position.z == 5.0f);
}

TEST_CASE("Dealer - Inventory", "[dealer]") {
    Dealer dealer({0, 0, 0});
    Inventory* inv = dealer.GetInventory();
    REQUIRE(inv != nullptr);
}

TEST_CASE("Dealer - Seating", "[dealer]") {
    Dealer dealer({0, 0, 0});
    
    SECTION("Not seated by default") {
        REQUIRE(dealer.IsSeated() == false);
    }
    
    SECTION("Can sit down") {
        dealer.SitDown({0, 1, 2.5f});
        REQUIRE(dealer.IsSeated() == true);
    }
}
