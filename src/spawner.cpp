#include "spawner.hpp"
#include "dom.hpp"
#include "physics.hpp"
#include <cmath>
#include <cstdlib>

Spawner::Spawner(Vector3 pos, float spawnRadius, Object* obj, int spawnCount)
    : Object(pos), radius(spawnRadius), templateObject(obj), count(spawnCount), hasSpawned(false)
{
    // Spawn immediately on construction
    PerformSpawn();
}

Spawner::~Spawner() {
    // Clean up the template object since we took ownership
    if (templateObject) {
        delete templateObject;
        templateObject = nullptr;
    }
}

void Spawner::PerformSpawn() {
    if (hasSpawned || !templateObject) return;

    DOM* dom = DOM::GetGlobal();
    if (!dom) return;

    // Use polymorphic Clone() - no type checking needed!
    for (int i = 0; i < count; i++) {
        // Random angle and distance
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float distance = ((float)(rand() % 100) / 100.0f) * radius;

        Vector3 spawnPos = {
            position.x + cosf(angle) * distance,
            position.y + (float)i * 0.2f,
            position.z + sinf(angle) * distance
        };

        // Clone the template object at the new position
        Object* clone = templateObject->Clone(spawnPos);
        if (clone) {
            dom->AddObject(clone);
        }
    }

    hasSpawned = true;
}

void Spawner::Update(float deltaTime) {
    // Spawner doesn't need to update after spawning
    (void)deltaTime;
}

std::string Spawner::GetType() const {
    return Object::GetType() + "_spawner";
}
