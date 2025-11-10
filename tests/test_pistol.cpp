#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/pistol.hpp"

TEST_CASE("Pistol - Construction", "[pistol]") {
    SECTION("Create pistol") {
        Pistol pistol({0, 0, 0}, nullptr);
        REQUIRE(pistol.position.x == 0.0f);
        REQUIRE(pistol.position.y == 0.0f);
        REQUIRE(pistol.position.z == 0.0f);
    }
}

TEST_CASE("Pistol - GetType", "[pistol]") {
    Pistol pistol({0, 0, 0}, nullptr);
    REQUIRE(pistol.GetType().find("pistol") != std::string::npos);
}

TEST_CASE("Pistol - Ammo", "[pistol]") {
    Pistol pistol({0, 0, 0}, nullptr);
    
    SECTION("Has initial ammo") {
        REQUIRE(pistol.GetAmmo() >= 0);
        REQUIRE(pistol.GetAmmo() <= pistol.maxAmmo);
    }
    
    SECTION("CanShoot when has ammo") {
        if (pistol.GetAmmo() > 0) {
            REQUIRE(pistol.CanShoot() == true);
        }
    }
    
    SECTION("Shoot decrements ammo") {
        int initialAmmo = pistol.GetAmmo();
        if (initialAmmo > 0) {
            pistol.Shoot();
            REQUIRE(pistol.GetAmmo() == initialAmmo - 1);
        }
    }
}
