#include "catch_amalgamated.hpp"
#include "weapons/pistol.hpp"
#include <string>

TEST_CASE("Pistol - Construction", "[pistol]") {
    SECTION("Create pistol") {
        Pistol pistol({0, 0, 0}, nullptr);
        REQUIRE(pistol.position.x == 0.0f);
        REQUIRE(pistol.position.y == 0.0f);
        REQUIRE(pistol.position.z == 0.0f);
    }
    
    SECTION("Pistol has correct ammo capacity") {
        Pistol pistol({0, 0, 0}, nullptr);
        REQUIRE(pistol.GetAmmo() == 6);  // Revolver has 6 rounds
        REQUIRE(pistol.GetMaxAmmo() == 6);
    }
}

TEST_CASE("Pistol - GetType", "[pistol]") {
    Pistol pistol({0, 0, 0}, nullptr);
    std::string type = pistol.GetType();
    REQUIRE(type.find("pistol") != std::string::npos);
    REQUIRE(type.find("weapon") != std::string::npos);
    REQUIRE(type.find("item") != std::string::npos);
}

TEST_CASE("Pistol - Ammo", "[pistol]") {
    Pistol pistol({0, 0, 0}, nullptr);
    
    SECTION("Has initial ammo") {
        REQUIRE(pistol.GetAmmo() == 6);
        REQUIRE(pistol.GetAmmo() <= pistol.GetMaxAmmo());
    }
    
    SECTION("CanShoot when has ammo") {
        REQUIRE(pistol.CanShoot() == true);
    }
    
    SECTION("Shoot decrements ammo") {
        int initialAmmo = pistol.GetAmmo();
        pistol.Shoot();
        REQUIRE(pistol.GetAmmo() == initialAmmo - 1);
        REQUIRE(pistol.GetAmmo() == 5);
    }
    
    SECTION("Shoot multiple times") {
        pistol.Shoot();
        pistol.Shoot();
        pistol.Shoot();
        REQUIRE(pistol.GetAmmo() == 3);
    }
    
    SECTION("Empty pistol cannot shoot") {
        // Shoot all 6 rounds
        for (int i = 0; i < 6; i++) {
            pistol.Shoot();
        }
        REQUIRE(pistol.GetAmmo() == 0);
        REQUIRE(pistol.CanShoot() == false);
    }
    
    SECTION("Reload refills ammo") {
        pistol.Shoot();
        pistol.Shoot();
        pistol.Shoot();
        REQUIRE(pistol.GetAmmo() == 3);
        
        pistol.Reload();
        REQUIRE(pistol.GetAmmo() == 6);
    }
}

TEST_CASE("Pistol - Clone", "[pistol]") {
    SECTION("Clone creates new pistol at different position") {
        Pistol original({1, 2, 3}, nullptr);
        Object* cloned = original.Clone({5, 6, 7});
        
        REQUIRE(cloned != nullptr);
        REQUIRE(cloned->position.x == 5.0f);
        REQUIRE(cloned->position.y == 6.0f);
        REQUIRE(cloned->position.z == 7.0f);
        REQUIRE(cloned->GetType().find("pistol") != std::string::npos);
        
        delete cloned;
    }
}
