#include "spawner.h"
#include <stdlib.h>
#include <math.h>

void Spawner_Init(Spawner* spawner, Vector3 pos, float radius) {
    Object_Init(&spawner->base, pos);
    spawner->radius = radius;
}

void Spawner_SpawnCards(Spawner* spawner, Suit suit, Rank rank, int count, 
                        PhysicsWorld* physics, Card* outputArray, int* currentIndex) {
    Vector3 basePos = spawner->base.position;
    float radius = spawner->radius;
    
    // Spawn cards randomly within radius
    for (int i = 0; i < count; i++) {
        // Random angle and distance
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float distance = ((float)(rand() % 100) / 100.0f) * radius;
        
        Vector3 spawnPos = {
            basePos.x + cosf(angle) * distance,
            basePos.y + (float)i * 0.2f,  // Stack vertically
            basePos.z + sinf(angle) * distance
        };
        
        Card_Init(&outputArray[*currentIndex], suit, rank, spawnPos, NULL, physics);
        (*currentIndex)++;
    }
}

void Spawner_SpawnChips(Spawner* spawner, int value, int count, 
                        PhysicsWorld* physics, Chip* outputArray, int* currentIndex) {
    Vector3 basePos = spawner->base.position;
    float radius = spawner->radius;
    
    // Spawn chips randomly within radius
    for (int i = 0; i < count; i++) {
        // Random angle and distance
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float distance = ((float)(rand() % 100) / 100.0f) * radius;
        
        Vector3 spawnPos = {
            basePos.x + cosf(angle) * distance,
            basePos.y + (float)i * 0.15f,  // Stack vertically
            basePos.z + sinf(angle) * distance
        };
        
        Chip_Init(&outputArray[*currentIndex], value, spawnPos, NULL, physics);
        (*currentIndex)++;
    }
}
