#include "poker_table.h"
#include "rlgl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "player.h"  // For collision category defines

// Virtual function wrappers
static void PokerTable_UpdateVirtual(Object* self, float deltaTime) {
    (void)deltaTime;
    PokerTable_Update((PokerTable*)self);
}

static void PokerTable_DrawVirtual(Object* self, Camera3D camera) {
    PokerTable_Draw((PokerTable*)self, camera);
}

// Callback for when player interacts with the table
// Note: We'll need to get the player reference from elsewhere (global or passed differently)
static void PokerTable_OnInteract(Interactable* self) {
    PokerTable* table = (PokerTable*)self;
    
    // TODO: For now, just print a message
    // In the future, we'll need a way to get the player reference
    printf("Interacted with poker table! Players at table: %d/%d\n", 
           table->playerCount, MAX_PLAYERS);
    
    // Shuffle the deck when interacted with
    Deck_Shuffle(&table->deck);
}

void PokerTable_Init(PokerTable* table, Vector3 pos, Vector3 size, Color color, PhysicsWorld* physics) {
    // Initialize base interactable with callback
    Interactable_Init(&table->base, pos, PokerTable_OnInteract);
    
    // Set virtual function pointers
    table->base.base.getType = PokerTable_GetType;
    table->base.base.update = PokerTable_UpdateVirtual;
    table->base.base.draw = PokerTable_DrawVirtual;
    
    // Initialize players array
    table->playerCount = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        table->players[i] = NULL;
    }
    
    // Initialize deck
    Deck_Init(&table->deck);
    Deck_Shuffle(&table->deck);
    
    // Set table properties
    table->size = size;
    table->color = color;
    table->physics = physics;
    
    // Create ODE box geometry for collision
    if (physics != NULL) {
        // Create a box geometry (static, no body needed)
        table->geom = dCreateBox(physics->space, size.x, size.y, size.z);
        
        // Set the position of the geometry
        dGeomSetPosition(table->geom, pos.x, pos.y, pos.z);
        
        // Set collision category and mask
        // Table collides with PLAYER only (not with ITEM)
        dGeomSetCategoryBits(table->geom, COLLISION_CATEGORY_TABLE);
        dGeomSetCollideBits(table->geom, COLLISION_CATEGORY_PLAYER);
        
        // Store a reference to the table in the geom for collision callbacks
        dGeomSetData(table->geom, table);
    } else {
        table->geom = NULL;
    }
}

void PokerTable_Update(PokerTable* table) {
    if (!table->base.isActive) return;
    
    // Future: update game logic, deal cards, etc.
}

void PokerTable_Draw(PokerTable* table, Camera3D camera) {
    (void)camera;
    if (!table->base.isActive) return;
    
    Vector3 pos = table->base.base.position;
    
    // Draw the table as a brown slab (rectangular box)
    DrawCube(pos, table->size.x, table->size.y, table->size.z, table->color);
    DrawCubeWires(pos, table->size.x, table->size.y, table->size.z, DARKBROWN);
    
    // Optional: Draw a green felt top (slightly above the table surface)
    Vector3 feltPos = pos;
    feltPos.y += table->size.y / 2 + 0.01f;  // Slightly above
    DrawCube(feltPos, table->size.x * 0.9f, 0.01f, table->size.z * 0.9f, DARKGREEN);
}

bool PokerTable_AddPlayer(PokerTable* table, Player* player) {
    // Check if table is full
    if (table->playerCount >= MAX_PLAYERS) {
        return false;
    }
    
    // Check if player is already at the table
    if (PokerTable_HasPlayer(table, player)) {
        return false;
    }
    
    // Add player to first available slot
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (table->players[i] == NULL) {
            table->players[i] = player;
            table->playerCount++;
            return true;
        }
    }
    
    return false;
}

void PokerTable_RemovePlayer(PokerTable* table, Player* player) {
    // Find and remove the player
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (table->players[i] == player) {
            table->players[i] = NULL;
            table->playerCount--;
            printf("Player left the table. Players remaining: %d/%d\n", 
                   table->playerCount, MAX_PLAYERS);
            return;
        }
    }
}

bool PokerTable_HasPlayer(PokerTable* table, Player* player) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (table->players[i] == player) {
            return true;
        }
    }
    return false;
}

void PokerTable_Cleanup(PokerTable* table) {
    // Destroy ODE geometry if it exists
    if (table->geom != NULL) {
        dGeomDestroy(table->geom);
        table->geom = NULL;
    }
    
    // Cleanup the deck
    Deck_Cleanup(&table->deck);
    
    // Clear player references (don't free players, they're owned elsewhere)
    for (int i = 0; i < MAX_PLAYERS; i++) {
        table->players[i] = NULL;
    }
    table->playerCount = 0;
}

const char* PokerTable_GetType(Object* obj) {
    (void)obj;
    return "poker_table";
}

void PokerTable_InteractWithPlayer(PokerTable* table, Player* player) {
    // Check if player is already at the table
    if (PokerTable_HasPlayer(table, player)) {
        printf("You are already seated at this table.\n");
        return;
    }
    
    // Try to add player to the table
    if (PokerTable_AddPlayer(table, player)) {
        printf("You joined the poker table! Players at table: %d/%d\n", 
               table->playerCount, MAX_PLAYERS);
        
        // Shuffle the deck when a player joins
        Deck_Shuffle(&table->deck);
    } else {
        printf("Table is full! (%d/%d players)\n", table->playerCount, MAX_PLAYERS);
    }
}
