#include "weapons/weapon.hpp"
#include "entities/person.hpp"
#include "core/dom.hpp"
#include <cmath>

Weapon::Weapon(Vector3 pos, int initialAmmo, int maxAmmoCapacity, PhysicsWorld* physics)
    : Item(pos), ammo(initialAmmo), maxAmmo(maxAmmoCapacity), rigidBody(nullptr)
{
    usable = true;  // Weapons can be used
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

void Weapon::Use() {
    if (ammo > 0) {
        ammo--;
    }
}

void Weapon::Shoot() {
    Use();  // Legacy method - just call Use()
}

const char* Weapon::GetDisplayCount(int stackCount) const {
    (void)stackCount;  // Unused - weapons show ammo, not stack count
    return TextFormat("%d", ammo);
}

Person* Weapon::PerformRaycast(Vector3 rayStart, Vector3 rayDirection, Person* shooter) {
    // Cast ray through all people in the DOM
    DOM* dom = DOM::GetGlobal();
    if (!dom) return nullptr;

    float maxDistance = 1000.0f;  // Max shooting range
    Person* hitPerson = nullptr;
    float closestHit = maxDistance;

    for (int i = 0; i < dom->GetCount(); i++) {
        Object* obj = dom->GetObject(i);
        if (!obj) continue;

        std::string type = obj->GetType();
        if (type.find("person") != std::string::npos) {
            Person* person = static_cast<Person*>(obj);
            
            // Don't shoot yourself!
            if (person == shooter) continue;

            // Cylinder collision: check if ray intersects the person's hitbox
            float personHeight = person->GetHeight();
            float personTopY = person->position.y + 2.4f * personHeight;
            float personBottomY = person->position.y;
            float hitRadius = 0.5f;

            // For each point along the ray, check cylinder intersection
            for (float dist = 0; dist < maxDistance; dist += 0.1f) {
                Vector3 rayPoint = {
                    rayStart.x + rayDirection.x * dist,
                    rayStart.y + rayDirection.y * dist,
                    rayStart.z + rayDirection.z * dist
                };

                // Check if within height range
                if (rayPoint.y >= personBottomY && rayPoint.y <= personTopY) {
                    // Check horizontal distance
                    float dx = rayPoint.x - person->position.x;
                    float dz = rayPoint.z - person->position.z;
                    float horizontalDist = sqrtf(dx*dx + dz*dz);

                    if (horizontalDist < hitRadius && dist < closestHit) {
                        hitPerson = person;
                        closestHit = dist;
                        break;  // Hit this person, stop checking this person
                    }
                }
            }
        }
    }

    return hitPerson;
}
