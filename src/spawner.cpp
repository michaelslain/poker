#include "spawner.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "pistol.hpp"
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

    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    // Physics can be nullptr - objects will handle it

    std::string objType = templateObject->GetType();

    // Spawn based on object type
    if (objType.find("card_") == 0) {
        // It's a card - extract suit and rank from the template
        Card* templateCard = static_cast<Card*>(templateObject);

        for (int i = 0; i < count; i++) {
            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
            float distance = ((float)(rand() % 100) / 100.0f) * radius;

            Vector3 spawnPos = {
                position.x + cosf(angle) * distance,
                position.y + (float)i * 0.2f,
                position.z + sinf(angle) * distance
            };

            Card* card = new Card(templateCard->suit, templateCard->rank, spawnPos, physics);
            dom->AddObject(card);
        }
    }
    else if (objType.find("chip_") == 0) {
        // It's a chip - extract value from the template
        Chip* templateChip = static_cast<Chip*>(templateObject);

        for (int i = 0; i < count; i++) {
            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
            float distance = ((float)(rand() % 100) / 100.0f) * radius;

            Vector3 spawnPos = {
                position.x + cosf(angle) * distance,
                position.y + (float)i * 0.15f,
                position.z + sinf(angle) * distance
            };

            Chip* chip = new Chip(templateChip->value, spawnPos, physics);
            dom->AddObject(chip);
        }
    }
    else if (objType == "pistol") {
        for (int i = 0; i < count; i++) {
            float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
            float distance = ((float)(rand() % 100) / 100.0f) * radius;

            Vector3 spawnPos = {
                position.x + cosf(angle) * distance,
                position.y + (float)i * 0.2f,
                position.z + sinf(angle) * distance
            };

            Pistol* pistol = new Pistol(spawnPos, physics);
            dom->AddObject(pistol);
        }
    }

    hasSpawned = true;
}

void Spawner::Update(float deltaTime) {
    // Spawner doesn't need to update after spawning
    (void)deltaTime;
}

std::string Spawner::GetType() const {
    return "spawner";
}
