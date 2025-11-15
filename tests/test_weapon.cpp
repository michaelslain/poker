#include "catch_amalgamated.hpp"
#include "weapons/weapon.hpp"
#include "weapons/pistol.hpp"
#include "core/physics.hpp"

// Concrete test weapon class for testing abstract Weapon class
class TestWeapon : public Weapon {
public:
    TestWeapon(Vector3 pos, int ammo, int maxAmmo)
        : Weapon(pos, ammo, maxAmmo, nullptr) {}
    
    void Draw(Camera3D camera) override {
        (void)camera;
        // Test implementation - do nothing
    }
    
    void DrawIcon(Rectangle destRect) override {
        (void)destRect;
        // Test implementation - do nothing
    }
    
    void DrawHeld(Camera3D camera) override {
        (void)camera;
        // Test implementation - do nothing
    }
    
    std::string GetType() const override {
        return Weapon::GetType() + "_test_weapon";
    }
    
    Object* Clone(Vector3 newPos) const override {
        return new TestWeapon(newPos, GetAmmo(), GetMaxAmmo());
    }
};

TEST_CASE("Weapon - Construction", "[weapon]") {
    SECTION("Default construction with ammo") {
        TestWeapon weapon({0, 0, 0}, 10, 15);
        
        REQUIRE(weapon.GetAmmo() == 10);
        REQUIRE(weapon.GetMaxAmmo() == 15);
        REQUIRE(weapon.CanShoot() == true);
    }
    
    SECTION("Construction with no ammo") {
        TestWeapon weapon({0, 0, 0}, 0, 10);
        
        REQUIRE(weapon.GetAmmo() == 0);
        REQUIRE(weapon.CanShoot() == false);
    }
    
    SECTION("Construction with full ammo") {
        TestWeapon weapon({0, 0, 0}, 20, 20);
        
        REQUIRE(weapon.GetAmmo() == 20);
        REQUIRE(weapon.GetMaxAmmo() == 20);
    }
}

TEST_CASE("Weapon - Shooting", "[weapon]") {
    SECTION("Shoot decrements ammo") {
        TestWeapon weapon({0, 0, 0}, 5, 10);
        
        weapon.Shoot();
        REQUIRE(weapon.GetAmmo() == 4);
        
        weapon.Shoot();
        REQUIRE(weapon.GetAmmo() == 3);
    }
    
    SECTION("Shoot when empty does not go negative") {
        TestWeapon weapon({0, 0, 0}, 1, 10);
        
        weapon.Shoot();
        REQUIRE(weapon.GetAmmo() == 0);
        REQUIRE(weapon.CanShoot() == false);
        
        weapon.Shoot();  // Should not decrement below 0
        REQUIRE(weapon.GetAmmo() == 0);
    }
    
    SECTION("CanShoot returns correct value") {
        TestWeapon weapon({0, 0, 0}, 1, 10);
        
        REQUIRE(weapon.CanShoot() == true);
        weapon.Shoot();
        REQUIRE(weapon.CanShoot() == false);
    }
}

TEST_CASE("Weapon - Ammo Management", "[weapon]") {
    SECTION("SetAmmo clamps to valid range") {
        TestWeapon weapon({0, 0, 0}, 5, 10);
        
        weapon.SetAmmo(7);
        REQUIRE(weapon.GetAmmo() == 7);
        
        weapon.SetAmmo(15);  // Above max
        REQUIRE(weapon.GetAmmo() == 10);  // Clamped to max
        
        weapon.SetAmmo(-5);  // Below 0
        REQUIRE(weapon.GetAmmo() == 0);  // Clamped to 0
    }
    
    SECTION("Reload fills to max ammo") {
        TestWeapon weapon({0, 0, 0}, 3, 10);
        
        weapon.Reload();
        REQUIRE(weapon.GetAmmo() == 10);
        
        weapon.Shoot();
        weapon.Shoot();
        REQUIRE(weapon.GetAmmo() == 8);
        
        weapon.Reload();
        REQUIRE(weapon.GetAmmo() == 10);
    }
}

TEST_CASE("Weapon - Type System", "[weapon]") {
    SECTION("Weapon returns hierarchical type") {
        TestWeapon weapon({0, 0, 0}, 10, 10);
        
        std::string type = weapon.GetType();
        REQUIRE(type.find("object") != std::string::npos);
        REQUIRE(type.find("interactable") != std::string::npos);
        REQUIRE(type.find("item") != std::string::npos);
        REQUIRE(type.find("weapon") != std::string::npos);
    }
}

TEST_CASE("Pistol - Inherits from Weapon", "[pistol][weapon]") {
    SECTION("Pistol has correct ammo") {
        Pistol pistol({0, 0, 0}, nullptr);
        
        REQUIRE(pistol.GetAmmo() == 6);  // Revolver capacity
        REQUIRE(pistol.GetMaxAmmo() == 6);
    }
    
    SECTION("Pistol shooting works") {
        Pistol pistol({0, 0, 0}, nullptr);
        
        pistol.Shoot();
        REQUIRE(pistol.GetAmmo() == 5);
        
        pistol.Shoot();
        pistol.Shoot();
        REQUIRE(pistol.GetAmmo() == 3);
    }
    
    SECTION("Pistol can be reloaded") {
        Pistol pistol({0, 0, 0}, nullptr);
        
        pistol.Shoot();
        pistol.Shoot();
        pistol.Shoot();
        REQUIRE(pistol.GetAmmo() == 3);
        
        pistol.Reload();
        REQUIRE(pistol.GetAmmo() == 6);
    }
    
    SECTION("Pistol type hierarchy") {
        Pistol pistol({0, 0, 0}, nullptr);
        
        std::string type = pistol.GetType();
        REQUIRE(type.find("weapon") != std::string::npos);
        REQUIRE(type.find("pistol") != std::string::npos);
    }
}
