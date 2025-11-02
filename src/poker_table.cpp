#include "poker_table.hpp"
#include "player.hpp"
#include "raymath.h"
#include <cstdio>

PokerTable::PokerTable(Vector3 pos, Vector3 tableSize, Color tableColor, PhysicsWorld* physicsWorld)
    : Interactable(pos), playerCount(0), size(tableSize), color(tableColor), geom(nullptr), physics(physicsWorld)
{
    // Initialize players array
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i] = nullptr;
    }
    
    // Initialize 8 seats around the table (2 per side)
    // Table layout:
    //     seat6   seat7
    //   +-----------+
    // s4|           |s5
    // e |           |e
    // a |           |a
    // t2|           |t3
    //   +-----------+
    //     seat0   seat1
    
    float halfWidth = size.x / 2.0f;
    float halfDepth = size.z / 2.0f;
    float seatDistance = 1.2f;  // Distance from table edge to seat position
    float groundLevel = 0.0f;   // Seats should be at ground level, not table height
    
    // Bottom side (facing +Z): seats 0, 1
    seats[0].position = {pos.x - halfWidth/2, groundLevel, pos.z + halfDepth + seatDistance};
    seats[1].position = {pos.x + halfWidth/2, groundLevel, pos.z + halfDepth + seatDistance};
    
    // Left side (facing -X): seats 2, 3
    seats[2].position = {pos.x - halfWidth - seatDistance, groundLevel, pos.z + halfDepth/2};
    seats[3].position = {pos.x - halfWidth - seatDistance, groundLevel, pos.z - halfDepth/2};
    
    // Right side (facing +X): seats 4, 5
    seats[4].position = {pos.x + halfWidth + seatDistance, groundLevel, pos.z + halfDepth/2};
    seats[5].position = {pos.x + halfWidth + seatDistance, groundLevel, pos.z - halfDepth/2};
    
    // Top side (facing -Z): seats 6, 7
    seats[6].position = {pos.x - halfWidth/2, groundLevel, pos.z - halfDepth - seatDistance};
    seats[7].position = {pos.x + halfWidth/2, groundLevel, pos.z - halfDepth - seatDistance};
    
    // Mark all seats as unoccupied
    for (int i = 0; i < MAX_SEATS; i++) {
        seats[i].occupant = nullptr;
        seats[i].isOccupied = false;
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
    // Check if player is already seated
    if (IsPlayerSeated(player)) {
        printf("You are already seated at this table.\n");
        UnseatPlayer(player);
        RemovePlayer(player);
        return;
    }
    
    // Find closest open seat
    int seatIndex = FindClosestOpenSeat(player->GetPosition());
    if (seatIndex == -1) {
        printf("Table is full! (%d/%d seats occupied)\n", playerCount, MAX_SEATS);
        return;
    }
    
    // Seat the player
    if (SeatPlayer(player, seatIndex) && AddPlayer(player)) {
        printf("You sat down at seat %d! Players at table: %d/%d\n", seatIndex, playerCount, MAX_PLAYERS);
        deck.Shuffle();
    }
}

int PokerTable::FindClosestOpenSeat(Vector3 playerPos) {
    int closestSeat = -1;
    float closestDist = FLT_MAX;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (!seats[i].isOccupied) {
            float dist = Vector3Distance(playerPos, seats[i].position);
            if (dist < closestDist) {
                closestDist = dist;
                closestSeat = i;
            }
        }
    }
    
    return closestSeat;
}

bool PokerTable::SeatPlayer(Player* player, int seatIndex) {
    if (seatIndex < 0 || seatIndex >= MAX_SEATS) {
        return false;
    }
    
    if (seats[seatIndex].isOccupied) {
        return false;
    }
    
    // Mark seat as occupied
    seats[seatIndex].occupant = player;
    seats[seatIndex].isOccupied = true;
    
    // Move player to seat and lock their position
    player->SitDown(seats[seatIndex].position);
    
    return true;
}

void PokerTable::UnseatPlayer(Player* player) {
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].occupant == player) {
            seats[i].occupant = nullptr;
            seats[i].isOccupied = false;
            player->StandUp();  // Allow player to move again
            printf("You left your seat.\n");
            return;
        }
    }
}

bool PokerTable::IsPlayerSeated(Player* player) {
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].occupant == player) {
            return true;
        }
    }
    return false;
}
