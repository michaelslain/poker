#include "poker_table.hpp"
#include "player.hpp"
#include <cstdio>

PokerTable::PokerTable(Vector3 pos, Vector3 tableSize, Color tableColor, PhysicsWorld* physicsWorld)
    : Interactable(pos), playerCount(0), size(tableSize), color(tableColor), geom(nullptr), physics(physicsWorld)
{
    // Initialize players array
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i] = nullptr;
    }
    
    // Deck is automatically initialized by its constructor
    deck.Shuffle();
    
    // Create ODE box geometry for collision
    if (physics != nullptr) {
        geom = dCreateBox(physics->space, size.x, size.y, size.z);
        dGeomSetPosition(geom, pos.x, pos.y, pos.z);
        
        // Set collision category and mask
        dGeomSetCategoryBits(geom, COLLISION_CATEGORY_TABLE);
        dGeomSetCollideBits(geom, COLLISION_CATEGORY_PLAYER);
        
        // Store a reference to the table in the geom
        dGeomSetData(geom, this);
    }
}

PokerTable::~PokerTable() {
    if (geom != nullptr) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    
    // Clear player references (don't delete players, they're owned elsewhere)
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i] = nullptr;
    }
    playerCount = 0;
}

void PokerTable::Update(float deltaTime) {
    (void)deltaTime;
    if (!isActive) return;
    
    // Future: update game logic, deal cards, etc.
}

void PokerTable::Draw(Camera3D camera) {
    (void)camera;
    if (!isActive) return;
    
    // Draw the table as a brown slab
    DrawCube(position, size.x, size.y, size.z, color);
    
    // Draw a green felt top (slightly above the table surface)
    Vector3 feltPos = position;
    feltPos.y += size.y / 2 + 0.01f;
    DrawCube(feltPos, size.x * 0.9f, 0.01f, size.z * 0.9f, DARKGREEN);
}

void PokerTable::Interact() {
    printf("Interacted with poker table! Players at table: %d/%d\n", playerCount, MAX_PLAYERS);
    deck.Shuffle();
}

const char* PokerTable::GetType() const {
    return "poker_table";
}

bool PokerTable::AddPlayer(Player* player) {
    if (playerCount >= MAX_PLAYERS) {
        return false;
    }
    
    if (HasPlayer(player)) {
        return false;
    }
    
    // Add player to first available slot
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i] == nullptr) {
            players[i] = player;
            playerCount++;
            return true;
        }
    }
    
    return false;
}

void PokerTable::RemovePlayer(Player* player) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i] == player) {
            players[i] = nullptr;
            playerCount--;
            printf("Player left the table. Players remaining: %d/%d\n", playerCount, MAX_PLAYERS);
            return;
        }
    }
}

bool PokerTable::HasPlayer(Player* player) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i] == player) {
            return true;
        }
    }
    return false;
}

void PokerTable::InteractWithPlayer(Player* player) {
    if (HasPlayer(player)) {
        printf("You are already seated at this table.\n");
        return;
    }
    
    if (AddPlayer(player)) {
        printf("You joined the poker table! Players at table: %d/%d\n", playerCount, MAX_PLAYERS);
        deck.Shuffle();
    } else {
        printf("Table is full! (%d/%d players)\n", playerCount, MAX_PLAYERS);
    }
}
