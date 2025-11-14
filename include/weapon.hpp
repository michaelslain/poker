#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "item.hpp"
#include "rigidbody.hpp"
#include "physics.hpp"

class Weapon : public Item {
protected:
    int ammo;
    int maxAmmo;
    RigidBody* rigidBody;

public:
    Weapon(Vector3 pos, int initialAmmo, int maxAmmoCapacity, PhysicsWorld* physics = nullptr);
    virtual ~Weapon();

    // Override virtual functions
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override = 0;  // Pure virtual - subclasses must implement
    void DrawIcon(Rectangle destRect) override = 0;  // Pure virtual - subclasses must implement
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override = 0;  // Pure virtual - subclasses must specify concrete type

    // Weapon-specific methods
    void DrawHeld(Camera3D camera) override = 0;  // Pure virtual - subclasses must implement mesh
    void Use() override;  // Shoot the weapon (decrements ammo) - called by Item::Use()
    void Shoot();  // Legacy method - calls Use()
    bool CanShoot() const { return ammo > 0; }
    
    // Raycast from the weapon to check for hits
    // Returns the Person* that was hit, or nullptr if no hit
    // shooter parameter prevents hitting yourself
    class Person* PerformRaycast(Vector3 rayStart, Vector3 rayDirection, class Person* shooter = nullptr);

    // Accessors
    int GetAmmo() const { return ammo; }
    int GetMaxAmmo() const { return maxAmmo; }
    void SetAmmo(int newAmmo) { ammo = newAmmo > maxAmmo ? maxAmmo : (newAmmo < 0 ? 0 : newAmmo); }
    void Reload() { ammo = maxAmmo; }
    
    // Weapons don't stack - each weapon is unique
    bool CanStack() const override { return false; }
    
    // Display ammo count instead of stack count
    const char* GetDisplayCount(int stackCount) const override;
};

#endif
