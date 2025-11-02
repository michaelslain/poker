#include "poker_table.hpp"
#include "person.hpp"
#include "dealer.hpp"
#include "chip.hpp"
#include "dom.hpp"
#include "raymath.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

PokerTable::PokerTable(Vector3 pos, Vector3 tableSize, Color tableColor, PhysicsWorld* physicsWorld)
    : Interactable(pos), size(tableSize), color(tableColor), geom(nullptr), physics(physicsWorld),
      handInProgress(false), currentStreet(0), dealerButtonSeat(-1), smallBlindSeat(-1), bigBlindSeat(-1), pot(0),
      bettingInProgress(false), currentBettorSeat(-1), streetBet(0), lastRaiserSeat(-1), lastAggressorSeat(-1)
{
    // Initialize seats around the table
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
    
    // IMPORTANT FIX: Use table's Y position, not hardcoded 0
    float groundLevel = pos.y - size.y / 2.0f;  // Ground is at bottom of table
    
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
    
    // Initialize all seats as unoccupied
    for (int i = 0; i < MAX_SEATS; i++) {
        seats[i].occupant = nullptr;
        seats[i].isOccupied = false;
        seats[i].chipCount = 0;
        seats[i].hasFolded = false;
        seats[i].isAllIn = false;
        seats[i].currentBet = 0;
    }
    
    // Initialize hole cards array
    for (int i = 0; i < MAX_SEATS * 2; i++) {
        holeCards[i] = nullptr;
    }
    
    // Create and initialize the deck
    Vector3 deckPos = {
        pos.x - halfWidth * 0.5f,
        pos.y + size.y / 2.0f + 0.05f,
        pos.z
    };
    deck = new Deck(deckPos);
    deck->Shuffle();
    deck->isDynamicallyAllocated = true;
    AddChild(deck);  // Deck is owned as child
    
    // Create the dealer at the center of one side of the table
    Vector3 dealerPos = {pos.x, groundLevel, pos.z - halfDepth - seatDistance};
    dealer = new Dealer(dealerPos, "Dealer");
    dealer->isDynamicallyAllocated = true;
    AddChild(dealer);  // Dealer is owned as child
    
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
    // Clear geom data pointer before destroying
    if (geom != nullptr) {
        dGeomSetData(geom, nullptr);
        dGeomDestroy(geom);
        geom = nullptr;
    }
    
    // Deck and dealer are children - they'll be cleaned up by parent destructor
    // Just nullify our pointers
    deck = nullptr;
    dealer = nullptr;
    
    // Clear seat references (don't delete persons, they're owned elsewhere)
    for (int i = 0; i < MAX_SEATS; i++) {
        seats[i].occupant = nullptr;
    }
    
    // Community cards and hole cards are children - will be cleaned up by parent
    communityCards.clear();
    for (int i = 0; i < MAX_SEATS * 2; i++) {
        holeCards[i] = nullptr;
    }
}

void PokerTable::Update(float deltaTime) {
    if (!isActive) return;
    
    // Process betting if a betting round is in progress
    if (bettingInProgress) {
        ProcessCurrentBet(deltaTime);
    }
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
    int occupiedCount = GetOccupiedSeatCount();
    printf("Interacted with poker table! Players at table: %d/%d\n", occupiedCount, MAX_SEATS);
    
    if (deck) {
        deck->Shuffle();
    }
}

const char* PokerTable::GetType() const {
    return "poker_table";
}

// ========== SEAT MANAGEMENT ==========

int PokerTable::FindClosestOpenSeat(Vector3 personPos) {
    int closestSeat = -1;
    float closestDist = FLT_MAX;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (!seats[i].isOccupied) {
            float dist = Vector3Distance(personPos, seats[i].position);
            if (dist < closestDist) {
                closestDist = dist;
                closestSeat = i;
            }
        }
    }
    
    return closestSeat;
}

bool PokerTable::SeatPerson(Person* person, int seatIndex) {
    if (seatIndex < 0 || seatIndex >= MAX_SEATS) {
        return false;
    }
    
    if (seats[seatIndex].isOccupied) {
        return false;
    }
    
    if (!person) {
        return false;
    }
    
    // Mark seat as occupied
    seats[seatIndex].occupant = person;
    seats[seatIndex].isOccupied = true;
    
    // If joining mid-hand, mark as folded so they can't act this hand
    if (handInProgress) {
        seats[seatIndex].hasFolded = true;  // Sit out current hand
        TraceLog(LOG_INFO, "PokerTable: %s seated at %d mid-hand, will play next hand", 
                 person->GetName().c_str(), seatIndex);
    } else {
        seats[seatIndex].hasFolded = false;
    }
    
    seats[seatIndex].isAllIn = false;
    seats[seatIndex].currentBet = 0;
    
    // Move person to seat facing the center of the table
    person->SitDownFacingPoint(seats[seatIndex].position, position);
    
    // Count chips in person's inventory
    seats[seatIndex].chipCount = CountChipsInInventory(person);
    
    TraceLog(LOG_INFO, "PokerTable: %s seated at %d with %d chips", 
             person->GetName().c_str(), seatIndex, seats[seatIndex].chipCount);
    
    // Check if we have at least 2 players to start a hand
    int occupiedCount = GetOccupiedSeatCount();
    
    if (occupiedCount >= 2 && !handInProgress) {
        TraceLog(LOG_INFO, "PokerTable: Starting new hand with %d players", occupiedCount);
        StartNewHand();
    }
    
    return true;
}

void PokerTable::UnseatPerson(Person* person) {
    if (!person) return;
    
    int seatIndex = FindSeatIndex(person);
    if (seatIndex == -1) return;
    
    // If hand is in progress, mark as folded
    if (handInProgress) {
        seats[seatIndex].hasFolded = true;
        TraceLog(LOG_INFO, "PokerTable: %s leaving mid-hand, marked as folded", person->GetName().c_str());
        
        // Check if only one player remains
        if (GetActiveSeatCount() <= 1) {
            TraceLog(LOG_INFO, "PokerTable: Only one player remains, ending hand");
            EndHand();
        }
    }
    
    // Clear seat
    seats[seatIndex].occupant = nullptr;
    seats[seatIndex].isOccupied = false;
    seats[seatIndex].chipCount = 0;
    seats[seatIndex].currentBet = 0;
    
    person->StandUp();  // Allow person to move again
    printf("%s left the table.\n", person->GetName().c_str());
}

int PokerTable::FindSeatIndex(Person* person) {
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].occupant == person) {
            return i;
        }
    }
    return -1;
}

// ========== CHIP MANAGEMENT ==========

int PokerTable::CountChipsInInventory(Person* person) {
    if (!person) return 0;
    
    Inventory* inv = person->GetInventory();
    if (!inv) return 0;
    
    int total = 0;
    int stackCount = inv->GetStackCount();
    
    for (int i = 0; i < stackCount; i++) {
        ItemStack* stack = inv->GetStack(i);
        if (stack && stack->item) {
            const char* type = stack->item->GetType();
            if (strncmp(type, "chip_", 5) == 0) {
                Chip* chip = static_cast<Chip*>(stack->item);
                total += chip->value * stack->count;
            }
        }
    }
    
    return total;
}

int PokerTable::RemoveChipsFromInventory(Person* person, int amount) {
    if (!person || amount <= 0) return 0;
    
    Inventory* inv = person->GetInventory();
    if (!inv) return 0;
    
    TraceLog(LOG_INFO, "RemoveChipsFromInventory(%s, %d): Starting", person->GetName().c_str(), amount);
    
    // Step 1: Count total chips person has
    int totalChips = CountChipsInInventory(person);
    
    if (totalChips < amount) {
        TraceLog(LOG_WARNING, "  Person only has %d chips, can't afford %d", totalChips, amount);
        amount = totalChips;  // Take all they have
    }
    
    // Step 2: Remove ALL chip items from inventory
    for (int i = inv->GetStackCount() - 1; i >= 0; i--) {
        ItemStack* stack = inv->GetStack(i);
        if (stack && stack->item) {
            const char* type = stack->item->GetType();
            if (strncmp(type, "chip_", 5) == 0) {
                // Remove all chips of this type
                int count = stack->count;
                for (int j = 0; j < count; j++) {
                    inv->RemoveItem(i);
                }
            }
        }
    }
    
    // Step 3: Calculate what they should have left
    int remaining = totalChips - amount;
    
    TraceLog(LOG_INFO, "  Removed all chips. Had %d, paying %d, left with %d", totalChips, amount, remaining);
    
    // Step 4: Give back optimal chip denominations
    if (remaining > 0) {
        AddChipsToInventory(person, remaining);
    }
    
    return amount;
}

void PokerTable::AddChipsToInventory(Person* person, int amount) {
    if (!person || amount <= 0) return;
    
    Inventory* inv = person->GetInventory();
    if (!inv) return;
    
    // Break amount into chip denominations
    int chipValues[] = {100, 25, 10, 5, 1};
    
    for (int value : chipValues) {
        while (amount >= value) {
            Chip* chip = new Chip(value, {0, 0, 0}, nullptr);  // No physics for inventory chips
            chip->isDynamicallyAllocated = true;
            inv->AddItem(chip);
            amount -= value;
        }
    }
}

// ========== POKER GAME METHODS ==========

int PokerTable::GetNextOccupiedSeat(int currentSeat) {
    if (currentSeat < 0 || currentSeat >= MAX_SEATS) return -1;
    
    // Search clockwise for next occupied seat
    for (int i = 1; i < MAX_SEATS; i++) {
        int nextSeat = (currentSeat + i) % MAX_SEATS;
        if (seats[nextSeat].isOccupied) {
            return nextSeat;
        }
    }
    return -1;  // No occupied seats found
}

int PokerTable::GetNextActiveSeat(int currentSeat) {
    if (currentSeat < 0 || currentSeat >= MAX_SEATS) return -1;
    
    // Search clockwise for next active seat (occupied and not folded)
    for (int i = 1; i < MAX_SEATS; i++) {
        int nextSeat = (currentSeat + i) % MAX_SEATS;
        if (seats[nextSeat].isOccupied && !seats[nextSeat].hasFolded) {
            return nextSeat;
        }
    }
    return -1;
}

int PokerTable::GetOccupiedSeatCount() {
    int count = 0;
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].isOccupied) count++;
    }
    return count;
}

int PokerTable::GetActiveSeatCount() {
    int count = 0;
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].isOccupied && !seats[i].hasFolded) count++;
    }
    return count;
}

void PokerTable::RotateDealerButton() {
    int occupiedCount = GetOccupiedSeatCount();
    
    if (occupiedCount < 2) {
        dealerButtonSeat = -1;
        return;
    }
    
    // If button is uninitialized or invalid, pick a random starting seat
    if (dealerButtonSeat < 0 || dealerButtonSeat >= MAX_SEATS || !seats[dealerButtonSeat].isOccupied) {
        // Find first occupied seat
        for (int i = 0; i < MAX_SEATS; i++) {
            if (seats[i].isOccupied) {
                dealerButtonSeat = i;
                TraceLog(LOG_INFO, "PokerTable: Dealer button initialized to seat %d", dealerButtonSeat);
                return;
            }
        }
    } else {
        // Move to next occupied seat
        int nextDealer = GetNextOccupiedSeat(dealerButtonSeat);
        if (nextDealer != -1) {
            dealerButtonSeat = nextDealer;
            TraceLog(LOG_INFO, "PokerTable: Dealer button moved to seat %d", dealerButtonSeat);
        }
    }
}

void PokerTable::AssignBlinds() {
    int occupiedCount = GetOccupiedSeatCount();
    
    if (occupiedCount < 2) {
        smallBlindSeat = -1;
        bigBlindSeat = -1;
        TraceLog(LOG_WARNING, "PokerTable: Not enough players for blinds");
        return;
    }
    
    // Heads-up (2 players): dealer is small blind, other player is big blind
    if (occupiedCount == 2) {
        smallBlindSeat = dealerButtonSeat;
        bigBlindSeat = GetNextOccupiedSeat(dealerButtonSeat);
        TraceLog(LOG_INFO, "PokerTable: Heads-up - Dealer/SB: %d, BB: %d", smallBlindSeat, bigBlindSeat);
    }
    // 3+ players: small blind is left of dealer, big blind is left of SB
    else {
        smallBlindSeat = GetNextOccupiedSeat(dealerButtonSeat);
        if (smallBlindSeat != -1) {
            bigBlindSeat = GetNextOccupiedSeat(smallBlindSeat);
        } else {
            bigBlindSeat = -1;
        }
        TraceLog(LOG_INFO, "PokerTable: Blinds assigned - Dealer: %d, SB: %d, BB: %d",
                 dealerButtonSeat, smallBlindSeat, bigBlindSeat);
    }
    
    if (smallBlindSeat == -1 || bigBlindSeat == -1) {
        TraceLog(LOG_ERROR, "PokerTable: Failed to assign blinds!");
    }
}

void PokerTable::CollectBlinds() {
    // Collect small blind
    if (smallBlindSeat >= 0 && smallBlindSeat < MAX_SEATS && seats[smallBlindSeat].isOccupied) {
        Person* person = seats[smallBlindSeat].occupant;
        if (person) {
            int collected = RemoveChipsFromInventory(person, SMALL_BLIND);
            seats[smallBlindSeat].currentBet = collected;
            seats[smallBlindSeat].chipCount = CountChipsInInventory(person);
            pot += collected;
            
            // Check if all-in
            if (seats[smallBlindSeat].chipCount == 0) {
                seats[smallBlindSeat].isAllIn = true;
            }
            
            TraceLog(LOG_INFO, "  Seat %d (%s) posts SB: %d (all-in: %s)", 
                     smallBlindSeat, person->GetName().c_str(), collected,
                     seats[smallBlindSeat].isAllIn ? "yes" : "no");
        }
    }
    
    // Collect big blind
    if (bigBlindSeat >= 0 && bigBlindSeat < MAX_SEATS && seats[bigBlindSeat].isOccupied) {
        Person* person = seats[bigBlindSeat].occupant;
        if (person) {
            int collected = RemoveChipsFromInventory(person, BIG_BLIND);
            seats[bigBlindSeat].currentBet = collected;
            seats[bigBlindSeat].chipCount = CountChipsInInventory(person);
            pot += collected;
            
            // Check if all-in
            if (seats[bigBlindSeat].chipCount == 0) {
                seats[bigBlindSeat].isAllIn = true;
            }
            
            TraceLog(LOG_INFO, "  Seat %d (%s) posts BB: %d (all-in: %s)", 
                     bigBlindSeat, person->GetName().c_str(), collected,
                     seats[bigBlindSeat].isAllIn ? "yes" : "no");
        }
    }
}

void PokerTable::ClearBoardAndHoleCards() {
    // Remove and delete community cards (they are table children)
    for (size_t i = 0; i < communityCards.size(); i++) {
        Card* card = communityCards[i];
        if (card) {
            RemoveChild(card);
            delete card;
        }
    }
    communityCards.clear();
    
    // Remove hole cards from player inventories and delete them
    for (int i = 0; i < MAX_SEATS * 2; i++) {
        if (holeCards[i]) {
            Card* card = holeCards[i];
            
            // Find which player has this card and remove it from their inventory
            for (int seatIdx = 0; seatIdx < MAX_SEATS; seatIdx++) {
                if (seats[seatIdx].isOccupied && seats[seatIdx].occupant) {
                    Inventory* inv = seats[seatIdx].occupant->GetInventory();
                    
                    // Search for this card in the inventory
                    for (int stackIdx = inv->GetStackCount() - 1; stackIdx >= 0; stackIdx--) {
                        ItemStack* stack = inv->GetStack(stackIdx);
                        if (stack && stack->item == card) {
                            inv->RemoveItem(stackIdx);
                            break;
                        }
                    }
                }
            }
            
            // Now delete the card
            delete card;
            holeCards[i] = nullptr;
        }
    }
}

void PokerTable::DealHoleCards() {
    // Deal 2 hole cards to each occupied seat
    // Cards are added to player inventories (not as table children)
    // holeCards array tracks them for game logic reference
    
    if (!deck) {
        TraceLog(LOG_ERROR, "PokerTable: Cannot deal hole cards - deck is null!");
        return;
    }
    
    int cardIndex = 0;
    
    // Deal one card at a time to each player (standard poker dealing)
    for (int round = 0; round < 2; round++) {
        for (int i = 0; i < MAX_SEATS; i++) {
            if (!seats[i].isOccupied || !seats[i].occupant) continue;
            
            Person* person = seats[i].occupant;
            Inventory* inv = person->GetInventory();
            
            Card* card = deck->DrawCard();
            if (!card) continue;
            
            // Store reference for game logic
            holeCards[cardIndex++] = card;
            
            // Add to player's inventory (cards don't render on table, only in UI)
            card->isActive = false;
            card->isDynamicallyAllocated = true;
            inv->AddItem(card);
        }
    }
}

void PokerTable::DealCommunityCard() {
    int currentCount = communityCards.size();
    
    if (currentCount >= 5) {
        TraceLog(LOG_WARNING, "PokerTable: Already dealt 5 community cards");
        return;
    }
    
    // Deal community card
    Card* card = deck->DrawCard();
    if (card) {
        // Position cards in center of table in a row
        float cardSpacing = 0.35f;
        float startX = position.x - (2.0f * cardSpacing);
        
        Vector3 cardPos = {
            startX + (currentCount * cardSpacing),
            position.y + size.y / 2.0f + 0.06f,
            position.z
        };
        
        card->position = cardPos;
        card->rotation = {90.0f, 0.0f, 0.0f};  // Lay flat face-up
        card->isActive = true;
        AddChild(card);  // Card is owned as child
        
        communityCards.push_back(card);
        
        const char* stageName = "Unknown";
        if (currentCount == 0) stageName = "Flop 1";
        else if (currentCount == 1) stageName = "Flop 2";
        else if (currentCount == 2) stageName = "Flop 3";
        else if (currentCount == 3) stageName = "Turn";
        else if (currentCount == 4) stageName = "River";
        
        TraceLog(LOG_INFO, "PokerTable: Dealt %s (%d/5 community cards)", stageName, (int)communityCards.size());
    }
}

void PokerTable::DealFlop() {
    TraceLog(LOG_INFO, "PokerTable: Dealing the flop...");
    for (int i = 0; i < 3; i++) {
        DealCommunityCard();
    }
}

void PokerTable::DealTurn() {
    if (communityCards.size() != 3) {
        TraceLog(LOG_WARNING, "PokerTable: Cannot deal turn, flop not complete (have %d cards)", 
                 (int)communityCards.size());
        return;
    }
    
    TraceLog(LOG_INFO, "PokerTable: Dealing the turn...");
    DealCommunityCard();
}

void PokerTable::DealRiver() {
    if (communityCards.size() != 4) {
        TraceLog(LOG_WARNING, "PokerTable: Cannot deal river, turn not complete (have %d cards)", 
                 (int)communityCards.size());
        return;
    }
    
    TraceLog(LOG_INFO, "PokerTable: Dealing the river...");
    DealCommunityCard();
}

void PokerTable::StartNewHand() {
    // Verify we have enough players
    int occupiedCount = GetOccupiedSeatCount();
    if (occupiedCount < 2) {
        TraceLog(LOG_WARNING, "PokerTable: Not enough players to start hand (%d/2)", occupiedCount);
        return;
    }
    
    TraceLog(LOG_INFO, "\n========================================");
    TraceLog(LOG_INFO, "    STARTING NEW HAND");
    TraceLog(LOG_INFO, "========================================");
    
    // Reset hand state
    handInProgress = true;
    currentStreet = 0;
    pot = 0;
    
    // Clear all cards from previous hand
    ClearBoardAndHoleCards();
    
    // Reset all seat states
    for (int i = 0; i < MAX_SEATS; i++) {
        seats[i].hasFolded = false;
        seats[i].isAllIn = false;
        seats[i].currentBet = 0;
        
        // Update chip count from inventory
        if (seats[i].isOccupied && seats[i].occupant) {
            seats[i].chipCount = CountChipsInInventory(seats[i].occupant);
        }
    }
    
    // Reset and shuffle deck
    deck->Reset();
    deck->Shuffle();
    
    // Rotate dealer button
    RotateDealerButton();
    
    // Assign blinds
    AssignBlinds();
    
    // Collect blinds
    TraceLog(LOG_INFO, "\n--- COLLECTING BLINDS ---");
    CollectBlinds();
    TraceLog(LOG_INFO, ">> Total pot after blinds: %d chips\n", pot);
    
    // Deal hole cards
    TraceLog(LOG_INFO, "--- DEALING HOLE CARDS ---");
    DealHoleCards();
    
    TraceLog(LOG_INFO, "\n--- HAND READY ---");
    TraceLog(LOG_INFO, "Pot: %d | Street: Preflop", pot);
    TraceLog(LOG_INFO, "========================================\n");
    
    // Start preflop betting
    StartBettingRound();
}

void PokerTable::EndHand() {
    TraceLog(LOG_INFO, "PokerTable: ========== ENDING HAND ==========");
    
    // Determine winner
    int winningSeat = DetermineWinner();
    
    // Award pot
    if (winningSeat != -1) {
        AwardPot(winningSeat);
    } else {
        TraceLog(LOG_WARNING, "PokerTable: No winner determined (tie or error)");
    }
    
    // Clear cards
    ClearBoardAndHoleCards();
    
    // Reset state
    handInProgress = false;
    bettingInProgress = false;
    currentStreet = 0;
    pot = 0;
    
    TraceLog(LOG_INFO, "PokerTable: Hand ended successfully");
    
    // Check if we have enough players to start a new hand
    int occupiedCount = GetOccupiedSeatCount();
    if (occupiedCount >= 2) {
        TraceLog(LOG_INFO, "PokerTable: Starting next hand with %d players\n", occupiedCount);
        StartNewHand();
    } else {
        TraceLog(LOG_INFO, "PokerTable: Not enough players to start next hand (%d/2)\n", occupiedCount);
    }
}

int PokerTable::DetermineWinner() {
    // Simplified winner determination:
    // - If only one player remains (all others folded), they win
    // - Otherwise, return first active player (TODO: implement hand evaluation)
    
    int activeSeat = -1;
    int activeCount = 0;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].isOccupied && !seats[i].hasFolded) {
            activeSeat = i;
            activeCount++;
        }
    }
    
    if (activeCount == 1) {
        TraceLog(LOG_INFO, "PokerTable: Seat %d wins by fold", activeSeat);
        return activeSeat;
    }
    
    if (activeCount == 0) {
        TraceLog(LOG_ERROR, "PokerTable: No active players remain!");
        return -1;
    }
    
    // TODO: Implement proper hand evaluation
    TraceLog(LOG_WARNING, "PokerTable: Hand evaluation not implemented, awarding to seat %d", activeSeat);
    return activeSeat;
}

void PokerTable::AwardPot(int winningSeat) {
    if (winningSeat < 0 || winningSeat >= MAX_SEATS) {
        TraceLog(LOG_ERROR, "PokerTable: Invalid winning seat: %d", winningSeat);
        return;
    }
    
    if (!seats[winningSeat].isOccupied || !seats[winningSeat].occupant) {
        TraceLog(LOG_ERROR, "PokerTable: Winning seat %d is not occupied", winningSeat);
        return;
    }
    
    Person* winner = seats[winningSeat].occupant;
    
    if (!winner) {
        TraceLog(LOG_ERROR, "PokerTable: Winner is nullptr at seat %d", winningSeat);
        return;
    }
    
    TraceLog(LOG_INFO, "PokerTable: Awarding pot of %d chips to seat %d (%s)", 
             pot, winningSeat, winner->GetName().c_str());
    
    // Only add chips if pot is non-zero
    if (pot > 0) {
        // Add chips to winner's inventory
        AddChipsToInventory(winner, pot);
        
        // Update seat chip count
        seats[winningSeat].chipCount = CountChipsInInventory(winner);
        
        printf("*** %s wins %d chips! ***\n", winner->GetName().c_str(), pot);
    } else {
        TraceLog(LOG_WARNING, "PokerTable: Pot is 0, no chips to award");
        printf("*** %s wins the hand (pot: 0 chips) ***\n", winner->GetName().c_str());
    }
    
    pot = 0;
}

void PokerTable::AdvanceToNextStreet() {
    if (!handInProgress) {
        TraceLog(LOG_WARNING, "PokerTable: No hand in progress, cannot advance street");
        return;
    }
    
    currentStreet++;
    
    switch (currentStreet) {
        case 1:  // Flop
            TraceLog(LOG_INFO, "PokerTable: ========== ADVANCING TO FLOP ==========");
            DealFlop();
            StartBettingRound();
            break;
        
        case 2:  // Turn
            TraceLog(LOG_INFO, "PokerTable: ========== ADVANCING TO TURN ==========");
            DealTurn();
            StartBettingRound();
            break;
        
        case 3:  // River
            TraceLog(LOG_INFO, "PokerTable: ========== ADVANCING TO RIVER ==========");
            DealRiver();
            StartBettingRound();
            break;
        
        case 4:  // Showdown
            TraceLog(LOG_INFO, "PokerTable: ========== SHOWDOWN ==========");
            EndHand();
            break;
        
        default:
            TraceLog(LOG_WARNING, "PokerTable: Invalid street: %d", currentStreet);
            break;
    }
}

// ========== BETTING METHODS ==========

void PokerTable::ResetStreetBets() {
    for (int i = 0; i < MAX_SEATS; i++) {
        seats[i].currentBet = 0;
    }
    streetBet = 0;
    lastRaiserSeat = -1;
}

void PokerTable::StartBettingRound() {
    TraceLog(LOG_INFO, "PokerTable: ========== STARTING BETTING ROUND ==========");
    
    bettingInProgress = true;
    
    // Reset bets for this street
    ResetStreetBets();
    
    int occupiedCount = GetOccupiedSeatCount();
    
    // On preflop, big blind is the current bet
    if (currentStreet == 0) {
        // SB and BB already posted
        seats[smallBlindSeat].currentBet = SMALL_BLIND;
        seats[bigBlindSeat].currentBet = BIG_BLIND;
        streetBet = BIG_BLIND;
        
        // Action starts left of BB
        currentBettorSeat = GetNextActiveSeat(bigBlindSeat);
        
        // BB is the last aggressor (gets option to raise if everyone calls)
        lastAggressorSeat = bigBlindSeat;
        lastRaiserSeat = bigBlindSeat;
    }
    // Post-flop streets
    else {
        streetBet = 0;
        
        // Action starts left of button
        if (occupiedCount == 2) {
            // Heads-up: button acts first post-flop
            currentBettorSeat = GetNextActiveSeat(bigBlindSeat);  // Start with SB (button)
        } else {
            // 3+: action starts left of button
            currentBettorSeat = GetNextActiveSeat(dealerButtonSeat);
        }
        
        lastAggressorSeat = -1;
        lastRaiserSeat = -1;
    }
    
    TraceLog(LOG_INFO, "PokerTable: Betting starts at seat %d, current bet: %d", currentBettorSeat, streetBet);
}

void PokerTable::ProcessCurrentBet(float deltaTime) {
    if (!bettingInProgress || currentBettorSeat == -1) return;
    
    // Check if betting is complete
    if (IsBettingComplete()) {
        bettingInProgress = false;
        TraceLog(LOG_INFO, "PokerTable: Betting round complete");
        AdvanceToNextStreet();
        return;
    }
    
    // Skip invalid seats
    if (currentBettorSeat < 0 || currentBettorSeat >= MAX_SEATS) {
        currentBettorSeat = GetNextActiveSeat(currentBettorSeat);
        return;
    }
    
    // Skip folded or all-in players
    if (seats[currentBettorSeat].hasFolded || seats[currentBettorSeat].isAllIn) {
        currentBettorSeat = GetNextActiveSeat(currentBettorSeat);
        return;
    }
    
    Person* currentPerson = seats[currentBettorSeat].occupant;
    if (!currentPerson) {
        currentBettorSeat = GetNextActiveSeat(currentBettorSeat);
        return;
    }
    
    // Calculate amounts
    int callAmount = streetBet - seats[currentBettorSeat].currentBet;
    int chipCount = seats[currentBettorSeat].chipCount;
    int minRaise = streetBet + BIG_BLIND;
    int maxRaise = seats[currentBettorSeat].currentBet + chipCount;  // All-in
    
    TraceLog(LOG_INFO, "  Seat %d (%s): streetBet=%d, currentBet=%d, callAmount=%d", 
             currentBettorSeat, currentPerson->GetName().c_str(), streetBet, 
             seats[currentBettorSeat].currentBet, callAmount);
    
    // Prompt for bet
    int raiseAmt = 0;
    int action = currentPerson->PromptBet(streetBet, callAmount, minRaise, maxRaise, raiseAmt);
    
    // If player/enemy hasn't decided yet, wait
    if (action == -1) return;
    
    // Execute the action
    ExecuteBettingAction(currentBettorSeat, action, raiseAmt);
    
    // Move to next player
    currentBettorSeat = GetNextActiveSeat(currentBettorSeat);
}

bool PokerTable::IsBettingComplete() {
    int activeCount = GetActiveSeatCount();
    
    // If only one player remains, betting is over
    if (activeCount <= 1) {
        return true;
    }
    
    // Count players who can still act
    int playersWhoCanAct = 0;
    int playersWhoHaveActed = 0;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].isOccupied && !seats[i].hasFolded) {
            // Can act if not all-in
            if (!seats[i].isAllIn) {
                playersWhoCanAct++;
                
                // Have acted if bet matches street bet
                if (seats[i].currentBet == streetBet) {
                    playersWhoHaveActed++;
                }
            }
        }
    }
    
    // Betting is complete if all players who can act have matched the bet
    return (playersWhoCanAct == playersWhoHaveActed);
}

void PokerTable::ExecuteBettingAction(int seatIndex, int action, int raiseAmt) {
    if (seatIndex < 0 || seatIndex >= MAX_SEATS) return;
    if (!seats[seatIndex].occupant) return;
    
    Person* person = seats[seatIndex].occupant;
    int callAmount = streetBet - seats[seatIndex].currentBet;
    
    switch (action) {
        case 0:  // Fold
            seats[seatIndex].hasFolded = true;
            TraceLog(LOG_INFO, "Seat %d (%s) folds", seatIndex, person->GetName().c_str());
            break;
        
        case 1:  // Call/Check
        {
            if (callAmount == 0) {
                // Check
                TraceLog(LOG_INFO, "Seat %d (%s) checks", seatIndex, person->GetName().c_str());
            } else {
                // Call
                int collected = RemoveChipsFromInventory(person, callAmount);
                seats[seatIndex].currentBet += collected;
                seats[seatIndex].chipCount = CountChipsInInventory(person);
                pot += collected;
                
                // Check if all-in
                if (seats[seatIndex].chipCount == 0) {
                    seats[seatIndex].isAllIn = true;
                    TraceLog(LOG_INFO, "Seat %d (%s) calls %d (ALL-IN)", seatIndex, person->GetName().c_str(), collected);
                } else {
                    TraceLog(LOG_INFO, "Seat %d (%s) calls %d", seatIndex, person->GetName().c_str(), collected);
                }
            }
            break;
        }
        
        case 2:  // Raise
        {
            int totalBet = raiseAmt;
            int needed = totalBet - seats[seatIndex].currentBet;
            int minRaiseAmt = streetBet + BIG_BLIND;
            
            // Validate raise
            if (totalBet < minRaiseAmt) {
                TraceLog(LOG_WARNING, "Seat %d: Invalid raise %d (min %d), treating as call", 
                         seatIndex, totalBet, minRaiseAmt);
                ExecuteBettingAction(seatIndex, 1, 0);  // Call instead
                return;
            }
            
            int collected = RemoveChipsFromInventory(person, needed);
            seats[seatIndex].currentBet += collected;
            seats[seatIndex].chipCount = CountChipsInInventory(person);
            pot += collected;
            streetBet = seats[seatIndex].currentBet;
            lastRaiserSeat = seatIndex;
            lastAggressorSeat = seatIndex;
            
            // Check if all-in
            if (seats[seatIndex].chipCount == 0) {
                seats[seatIndex].isAllIn = true;
                TraceLog(LOG_INFO, "Seat %d (%s) raises to %d (ALL-IN)", seatIndex, person->GetName().c_str(), streetBet);
            } else {
                TraceLog(LOG_INFO, "Seat %d (%s) raises to %d", seatIndex, person->GetName().c_str(), streetBet);
            }
            break;
        }
        
        default:
            TraceLog(LOG_WARNING, "PokerTable: Invalid action %d", action);
            break;
    }
}
