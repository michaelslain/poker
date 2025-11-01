#ifndef POKER_TABLE_H
#define POKER_TABLE_H

#include "interactable.h"
#include "deck.h"
#include "player.h"

#define MAX_PLAYERS 8  // Maximum players at a poker table

typedef struct {
    Interactable base;
    
    // Players at the table (array of Player pointers)
    Player* players[MAX_PLAYERS];
    int playerCount;
    
    // Deck for this table
    Deck deck;
    
    // Table dimensions
    Vector3 size;  // Width, height (thickness), depth
    Color color;
} PokerTable;

// Initialize poker table
void PokerTable_Init(PokerTable* table, Vector3 pos, Vector3 size, Color color);

// Update the table (if needed in the future)
void PokerTable_Update(PokerTable* table);

// Draw the table
void PokerTable_Draw(PokerTable* table, Camera3D camera);

// Add a player to the table
// Returns true if successful, false if table is full
bool PokerTable_AddPlayer(PokerTable* table, Player* player);

// Remove a player from the table
void PokerTable_RemovePlayer(PokerTable* table, Player* player);

// Check if a player is at the table
bool PokerTable_HasPlayer(PokerTable* table, Player* player);

// Cleanup the table (cleans up deck and player references)
void PokerTable_Cleanup(PokerTable* table);

// Get type string
const char* PokerTable_GetType(Object* obj);

// Manual interaction with player reference (call this from main loop)
void PokerTable_InteractWithPlayer(PokerTable* table, Player* player);

#endif // POKER_TABLE_H
