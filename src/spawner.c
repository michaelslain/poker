#include "spawner.h"
#include <stdlib.h>
#include <math.h>

void Spawner_Init(Spawner* spawner, Vector3 pos, float radius) {
    Object_Init(&spawner->base, pos);
    spawner->radius = radius;
}

void Spawner_SpawnCards(Spawner* spawner, Suit suit, Rank rank, int count, 
                        PhysicsWorld* physics, DOM* dom) {
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
        
        // Allocate a new card
        Card* card = (Card*)malloc(sizeof(Card));
        Card_Init(card, suit, rank, spawnPos, NULL, physics);
        card->base.base.base.isDynamicallyAllocated = true;
        
        // Add to DOM
        DOM_AddObject(dom, (Object*)card);
    }
}

void Spawner_SpawnChips(Spawner* spawner, int value, int count, 
                        PhysicsWorld* physics, DOM* dom) {
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
        
        // Allocate a new chip
        Chip* chip = (Chip*)malloc(sizeof(Chip));
        Chip_Init(chip, value, spawnPos, NULL, physics);
        chip->base.base.base.isDynamicallyAllocated = true;
        
        // Add to DOM
        DOM_AddObject(dom, (Object*)chip);
    }
}
