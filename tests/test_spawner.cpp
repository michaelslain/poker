#include "catch_amalgamated.hpp"
#include "../include/spawner.hpp"
#include "../include/dom.hpp"

TEST_CASE("Spawner - Construction", "[spawner]") {
    SECTION("Create spawner with radius") {
        Spawner spawner({0, 0, 0}, 5.0f);
        REQUIRE(spawner.position.x == 0.0f);
        REQUIRE(spawner.position.y == 0.0f);
        REQUIRE(spawner.position.z == 0.0f);
    }
}

TEST_CASE("Spawner - GetType", "[spawner]") {
    Spawner spawner({0, 0, 0}, 5.0f);
    REQUIRE(spawner.GetType() == "spawner");
}

TEST_CASE("Spawner - SpawnCards", "[spawner]") {
    Spawner spawner({0, 0, 0}, 5.0f);
    DOM dom;
    
    SECTION("Spawn single card") {
        int initialCount = dom.GetCount();
        spawner.SpawnCards(SUIT_SPADES, RANK_ACE, 1, nullptr, &dom);
        REQUIRE(dom.GetCount() == initialCount + 1);
    }
    
    SECTION("Spawn multiple cards") {
        int initialCount = dom.GetCount();
        spawner.SpawnCards(SUIT_HEARTS, RANK_KING, 5, nullptr, &dom);
        REQUIRE(dom.GetCount() == initialCount + 5);
    }
    
    // Cleanup
    for (int i = 0; i < dom.GetCount(); i++) {
        delete dom.GetObject(i);
    }
}

TEST_CASE("Spawner - SpawnChips", "[spawner]") {
    Spawner spawner({0, 0, 0}, 5.0f);
    DOM dom;
    
    SECTION("Spawn chips") {
        int initialCount = dom.GetCount();
        spawner.SpawnChips(10, 3, nullptr, &dom);
        REQUIRE(dom.GetCount() == initialCount + 3);
    }
    
    // Cleanup
    for (int i = 0; i < dom.GetCount(); i++) {
        delete dom.GetObject(i);
    }
}
