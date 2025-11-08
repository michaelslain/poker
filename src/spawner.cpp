#include "spawner.hpp"
#include "pistol.hpp"
#include <cmath>
#include <cstdlib>

Spawner::Spawner(Vector3 pos, float spawnRadius)
    : Object(pos), radius(spawnRadius)
{
}

std::string Spawner::GetType() const {
    return "spawner";
}

void Spawner::SpawnCards(Suit suit, Rank rank, int count, PhysicsWorld* physics, DOM* dom) {
    // Spawn cards randomly within radius
    for (int i = 0; i < count; i++) {
        // Random angle and distance
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float distance = ((float)(rand() % 100) / 100.0f) * radius;
        
        Vector3 spawnPos = {
            position.x + cosf(angle) * distance,
            position.y + (float)i * 0.2f,
            position.z + sinf(angle) * distance
        };
        
        // Create new card with C++ new
        Card* card = new Card(suit, rank, spawnPos, physics);
        
        // Add to DOM
        dom->AddObject(card);
    }
}

void Spawner::SpawnChips(int value, int count, PhysicsWorld* physics, DOM* dom) {
    // Spawn chips randomly within radius
    for (int i = 0; i < count; i++) {
        // Random angle and distance
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float distance = ((float)(rand() % 100) / 100.0f) * radius;
        
        Vector3 spawnPos = {
            position.x + cosf(angle) * distance,
            position.y + (float)i * 0.15f,
            position.z + sinf(angle) * distance
        };
        
        // Create new chip with C++ new
        Chip* chip = new Chip(value, spawnPos, physics);
        
        // Add to DOM
        dom->AddObject(chip);
    }
}

void Spawner::SpawnPistols(int count, PhysicsWorld* physics, DOM* dom) {
    // Spawn pistols randomly within radius
    for (int i = 0; i < count; i++) {
        // Random angle and distance
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float distance = ((float)(rand() % 100) / 100.0f) * radius;
        
        Vector3 spawnPos = {
            position.x + cosf(angle) * distance,
            position.y + (float)i * 0.2f,
            position.z + sinf(angle) * distance
        };
        
        // Create new pistol with C++ new
        Pistol* pistol = new Pistol(spawnPos, physics);
        
        // Add to DOM
        dom->AddObject(pistol);
    }
}
