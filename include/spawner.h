#ifndef SPAWNER_H
#define SPAWNER_H

#include "object.h"
#include "card.h"
#include "chip.h"
#include "physics.h"

typedef enum {
    SPAWN_TYPE_CARD,
    SPAWN_TYPE_CHIP
} SpawnType;

typedef struct {
    Object base;
    SpawnType spawnType;
    float radius;  // Spawn radius
    
    // For cards
    Suit cardSuit;
    Rank cardRank;
    
    // For chips
    int chipValue;
} Spawner;

void Spawner_Init(Spawner* spawner, Vector3 pos, float radius);

// Spawn cards
void Spawner_SpawnCards(Spawner* spawner, Suit suit, Rank rank, int count, 
                        PhysicsWorld* physics, Card* outputArray, int* currentIndex);

// Spawn chips
void Spawner_SpawnChips(Spawner* spawner, int value, int count, 
                        PhysicsWorld* physics, Chip* outputArray, int* currentIndex);

#endif
