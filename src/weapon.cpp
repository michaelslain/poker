#include "weapon.hpp"

Weapon::Weapon(Vector3 pos, int initialAmmo, int maxAmmoCapacity, PhysicsWorld* physics)
    : Item(pos), ammo(initialAmmo), maxAmmo(maxAmmoCapacity), rigidBody(nullptr)
{
    // Initialize physics if provided
    if (physics) {
        Vector3 weaponSize = {0.3f, 0.2f, 0.5f};  // Default weapon size
        rigidBody = new RigidBody(pos);
        rigidBody->InitBox(physics, pos, weaponSize, 0.5f);  // Moderate mass
    }
}

Weapon::~Weapon() {
    if (rigidBody) {
        delete rigidBody;
        rigidBody = nullptr;
    }
}

void Weapon::Update(float deltaTime) {
    (void)deltaTime;
    
    // Only sync if physics is attached
    if (rigidBody && rigidBody->body) {
        rigidBody->Update(deltaTime);
        position = rigidBody->position;
        rotation = rigidBody->rotation;
    }
}

std::string Weapon::GetType() const {
    return Item::GetType() + "_weapon";
}

void Weapon::Shoot() {
    if (ammo > 0) {
        ammo--;
    }
}
