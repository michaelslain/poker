#include "poker_table.hpp"
#include "dealer.hpp"
#include "player.hpp"
#include "chip.hpp"
#include "debug.hpp"
#include "dom.hpp"
#include "raymath.h"
#include <cstring>
#include <algorithm>
#include <map>
#include <cstdio>

PokerTable::PokerTable(Vector3 pos, Vector3 tableSize, Color tableColor, PhysicsWorld* physicsWorld)
    : Interactable(pos), size(tableSize), color(tableColor), geom(nullptr), physics(physicsWorld),
      dealer(nullptr), deck(nullptr), potStack(nullptr),
      smallBlindSeat(-1), bigBlindSeat(-1), currentPlayerSeat(-1),
      currentBet(0), potValue(0), handActive(false), bettingActive(false), showdownActive(false),
      lastLoggedPlayerSeat(-1)
{
    // Calculate seat positions around the table
    float hw = size.x / 2.0f;
    float hd = size.z / 2.0f;
    float dist = 1.2f;
    float ground = pos.y - size.y / 2.0f;
    
    seats[0].position = {pos.x - hw/2, ground, pos.z + hd + dist};
    seats[1].position = {pos.x + hw/2, ground, pos.z + hd + dist};
    seats[2].position = {pos.x - hw - dist, ground, pos.z + hd/2};
    seats[3].position = {pos.x - hw - dist, ground, pos.z - hd/2};
    seats[4].position = {pos.x + hw + dist, ground, pos.z + hd/2};
    seats[5].position = {pos.x + hw + dist, ground, pos.z - hd/2};
    seats[6].position = {pos.x - hw/2, ground, pos.z - hd - dist};
    seats[7].position = {pos.x + hw/2, ground, pos.z - hd - dist};
    
    // Initialize seats as empty
    for (int i = 0; i < MAX_SEATS; i++) {
        seats[i].occupant = nullptr;
        seats[i].isOccupied = false;
        statusList[i] = 0;
        hasRaised[i] = false;
    }
    
    // Create dealer and add to DOM
    dealer = new Dealer({pos.x, ground, pos.z - hd - dist}, "Dealer");
    DOM::GetGlobal()->AddObject(dealer);
    
    // Create deck and add to DOM
    Vector3 deckPos = {pos.x - hw * 0.5f, pos.y + size.y / 2.0f + 0.05f, pos.z};
    deck = new Deck(deckPos);
    deck->Shuffle();
    DOM::GetGlobal()->AddObject(deck);
    
    // Create pot stack and add to DOM
    Vector3 potPos = {pos.x - hw * 0.5f, pos.y + size.y / 2.0f + 0.05f, pos.z - 0.5f};
    potStack = new ChipStack(potPos);
    DOM::GetGlobal()->AddObject(potStack);
    
    // Create collision geometry
    if (physics) {
        geom = dCreateBox(physics->space, size.x, size.y, size.z);
        dGeomSetPosition(geom, pos.x, pos.y, pos.z);
        dGeomSetCategoryBits(geom, COLLISION_CATEGORY_TABLE);
        dGeomSetCollideBits(geom, COLLISION_CATEGORY_PLAYER);
        dGeomSetData(geom, this);
    }
}

PokerTable::~PokerTable() {
    // Clean up dealer (remove from DOM and delete)
    if (dealer) {
        DOM* dom = DOM::GetGlobal();
        if (dom) {
            dom->RemoveObject(dealer);
        }
        delete dealer;
        dealer = nullptr;
    }
    
    // Clean up pot stack (remove from DOM and delete)
    if (potStack) {
        DOM* dom = DOM::GetGlobal();
        if (dom) {
            dom->RemoveObject(potStack);
        }
        delete potStack;
        potStack = nullptr;
    }
    
    // Clean up deck (remove from DOM and delete)
    if (deck) {
        DOM* dom = DOM::GetGlobal();
        if (dom) {
            dom->RemoveObject(deck);
        }
        delete deck;
        deck = nullptr;
    }
    
    if (geom) {
        dGeomSetData(geom, nullptr);
        dGeomDestroy(geom);
        geom = nullptr;
    }
}

void PokerTable::Update(float deltaTime) {
    // Check if dealer is still alive (if they were shot, stop the game)
    if (dealer) {
        DOM* dom = DOM::GetGlobal();
        bool dealerExists = false;
        
        if (dom) {
            // Check if dealer is still in the DOM
            for (int i = 0; i < dom->GetCount(); i++) {
                if (dom->GetObject(i) == dealer) {
                    dealerExists = true;
                    break;
                }
            }
        }
        
        // If dealer was removed (shot), stop the game
        if (!dealerExists) {
            POKER_LOG(LOG_INFO, "*** DEALER WAS KILLED - POKER GAME STOPPED ***");
            dealer = nullptr;
            handActive = false;
            bettingActive = false;
            
            // Clear all seats
            for (int i = 0; i < MAX_SEATS; i++) {
                if (seats[i].isOccupied && seats[i].occupant) {
                    seats[i].occupant->StandUp();
                    seats[i].occupant = nullptr;
                    seats[i].isOccupied = false;
                }
            }
            return;
        }
    }
    
    // Start hand if we have 2+ players and no active hand and dealer is alive
    if (!handActive && GetOccupiedSeatCount() >= 2 && dealer) {
        StartHand();
    }
    
    // Process betting if active
    if (bettingActive) {
        ProcessBetting(deltaTime);
        POKER_LOG(LOG_INFO, "ProcessBetting completed, returning from PokerTable::Update");
    }
    
    // Process showdown if active (waiting for card selection)
    if (showdownActive) {
        Showdown();
    }
}

void PokerTable::Draw(Camera3D camera) {
    (void)camera;
    
    // Draw table body
    DrawCube(position, size.x, size.y, size.z, color);
    
    // Draw felt top
    DrawCube({position.x, position.y + size.y/2.0f, position.z},
             size.x * 0.9f, 0.01f, size.z * 0.8f, GREEN);
    
    // Draw collision box wireframe if debug mode is on
    if (g_showCollisionDebug) {
        DrawCubeWires(position, size.x, size.y, size.z, LIME);
    }
    
    // Community cards and deck are now managed by DOM - they render themselves
}

void PokerTable::Interact() {
    // POKER_LOG(LOG_INFO, "Table interaction");
}

std::string PokerTable::GetType() const {
    return "poker_table";
}

// ========== SEATING ==========

int PokerTable::FindClosestOpenSeat(Vector3 pos) {
    int closest = -1;
    float closestDist = FLT_MAX;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (!seats[i].isOccupied) {
            float dist = Vector3Distance(pos, seats[i].position);
            if (dist < closestDist) {
                closestDist = dist;
                closest = i;
            }
        }
    }
    
    return closest;
}

bool PokerTable::SeatPerson(Person* p, int seatIndex) {
    if (seatIndex < 0 || seatIndex >= MAX_SEATS) return false;
    if (!p) return false;
    if (seats[seatIndex].isOccupied) return false;
    
    // Can't sit down if dealer is dead
    if (!dealer) {
        POKER_LOG(LOG_INFO, "Cannot sit down - dealer has been killed!");
        return false;
    }
    
    // Seat the person
    p->SitDownFacingPoint(seats[seatIndex].position, position);
    seats[seatIndex].occupant = p;
    seats[seatIndex].isOccupied = true;
    
    // If joining mid-hand, mark as folded
    if (handActive) {
        statusList[seatIndex] = -1;
        // POKER_LOG(LOG_INFO, "Seated %s at seat %d (folded - mid-hand)", p->GetName().c_str(), seatIndex);
    } else {
        statusList[seatIndex] = 0;
        // POKER_LOG(LOG_INFO, "Seated %s at seat %d", p->GetName().c_str(), seatIndex);
    }
    
    return true;
}

void PokerTable::UnseatPerson(Person* p) {
    if (!p) return;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].occupant == p) {
            // Return only the dealt hole cards before unseating (during active hand)
            if (handActive) {
                Inventory* inv = p->GetInventory();
                if (inv) {
                    // Only remove the specific cards that were dealt this hand
                    std::string personName = p->GetName();
                    POKER_LOG(LOG_INFO, "Returning %d dealt cards for %s", (int)dealtHoleCards[i].size(), personName.c_str());
                    for (Card* dealtCard : dealtHoleCards[i]) {
                        // Find and remove this specific card from inventory
                        for (int j = inv->GetStackCount() - 1; j >= 0; j--) {
                            ItemStack* stack = inv->GetStack(j);
                            if (stack && stack->item == dealtCard) {
                                // Just remove from inventory - Deck owns the cards
                                inv->RemoveItem(j);
                                break;
                            }
                        }
                    }
                    // Clear this seat's dealt cards list
                    dealtHoleCards[i].clear();
                }
            }
            
            seats[i].occupant = nullptr;
            seats[i].isOccupied = false;
            statusList[i] = 0;
            p->StandUp();
            // POKER_LOG(LOG_INFO, "Unseated %s from seat %d", p->GetName().c_str(), i);
            return;
        }
    }
}

int PokerTable::FindSeatIndex(Person* p) {
    if (!p) return -1;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].occupant == p) {
            return i;
        }
    }
    
    return -1;
}

// ========== SEAT NAVIGATION ==========

int PokerTable::NextOccupiedSeat(int index) {
    for (int i = 1; i <= MAX_SEATS; i++) {
        int next = (index + i) % MAX_SEATS;
        if (seats[next].isOccupied) {
            return next;
        }
    }
    return -1;  // No occupied seats
}

int PokerTable::NextActiveSeat(int index) {
    for (int i = 1; i <= MAX_SEATS; i++) {
        int next = (index + i) % MAX_SEATS;
        if (seats[next].isOccupied && statusList[next] != -1) {  // Not folded
            return next;
        }
    }
    return -1;  // Everyone folded
}

int PokerTable::GetOccupiedSeatCount() {
    int count = 0;
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].isOccupied) count++;
    }
    return count;
}

// ========== CHIP MANAGEMENT ==========

int PokerTable::CountChips(Person* p) {
    if (!p) return 0;
    Inventory* inv = p->GetInventory();
    if (!inv) return 0;
    int total = 0;
    for (int i = 0; i < inv->GetStackCount(); i++) {
        ItemStack* stack = inv->GetStack(i);
        if (stack && stack->item && stack->item->GetType().substr(0, 5) == "chip_") {
            Chip* chip = static_cast<Chip*>(stack->item);
            total += chip->value * stack->count;
        }
    }
    return total;
}



std::vector<Chip*> PokerTable::CalculateChipCombination(int amount) {
    std::vector<Chip*> result;
    // Standard poker chip denominations (must match Chip::GetColorFromValue())
    // BLACK=100, GREEN=25, BLUE=10, RED=5, WHITE=1
    int denominations[] = {100, 25, 10, 5, 1};
    int originalAmount = amount;
    
    // Track chip breakdown for logging
    std::map<int, int> chipCounts;
    
    for (int denom : denominations) {
        int count = 0;
        while (amount >= denom) {
            Chip* chip = new Chip(denom, {0, 0, 0}, nullptr);
            result.push_back(chip);
            amount -= denom;
            count++;
        }
        if (count > 0) {
            chipCounts[denom] = count;
        }
    }
    
    // Log the chip breakdown
    if (!chipCounts.empty()) {
        std::string breakdown = "";
        for (auto& pair : chipCounts) {
            breakdown += std::to_string(pair.second) + "x" + std::to_string(pair.first) + " ";
        }
        // POKER_LOG(LOG_INFO, "Chip combination for %d: %s(total: %zu chips)", 
    //                   originalAmount, breakdown.c_str(), result.size());
    }
    
    return result;
}

void PokerTable::TakeChips(Person* p, int amount) {
    if (!p || amount <= 0) return;
    
    Inventory* inv = p->GetInventory();
    if (!inv) return;
    int totalChips = CountChips(p);
    
    if (amount > totalChips) amount = totalChips;  // All-in
    
    // Remove all chips from player
    for (int i = inv->GetStackCount() - 1; i >= 0; i--) {
        ItemStack* stack = inv->GetStack(i);
        if (stack && stack->item && stack->item->GetType().substr(0, 5) == "chip_") {
            for (int j = 0; j < stack->count; j++) {
                inv->RemoveItem(i);
            }
        }
    }
    
    // Give back change
    int remaining = totalChips - amount;
    if (remaining > 0) {
        GiveChips(p, remaining);
    }
    
    // Add bet to pot
    std::vector<Chip*> betChips = CalculateChipCombination(amount);
    potStack->AddChips(betChips);
    potValue += amount;
    
    // POKER_LOG(LOG_INFO, "%s bets %d chips (pot now: %d)", p->GetName().c_str(), amount, potValue);
}

void PokerTable::GiveChips(Person* p, int amount) {
    if (!p || amount <= 0) return;
    
    std::vector<Chip*> chips = CalculateChipCombination(amount);
    Inventory* inv = p->GetInventory();
    if (!inv) {
        // Clean up chips if we can't give them
        for (Chip* chip : chips) {
            delete chip;
        }
        return;
    }
    
    for (Chip* chip : chips) {
        inv->AddItem(chip);
    }
    
    // POKER_LOG(LOG_INFO, "Gave %d chips to %s", amount, p->GetName().c_str());
}

// ========== BETTING HELPERS ==========

bool PokerTable::IsBettingRoundComplete() {
    int activePlayers = 0;
    int lastBet = -1;
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (seats[i].isOccupied && statusList[i] != -1) {  // Not folded
            activePlayers++;
            if (lastBet == -1) {
                lastBet = statusList[i];
            } else if (statusList[i] != lastBet) {
                return false;  // Bets don't match
            }
        }
    }
    
    return activePlayers <= 1 || (lastBet >= 0);  // All bets match or only 1 player left
}

void PokerTable::ProcessBetting(float dt) {
    (void)dt;
    
    if (currentPlayerSeat < 0 || currentPlayerSeat >= MAX_SEATS) {
        bettingActive = false;
        return;
    }
    
    // Skip empty seats
    if (!seats[currentPlayerSeat].isOccupied) {
        currentPlayerSeat = NextActiveSeat(currentPlayerSeat);
        return;
    }
    
    Person* p = seats[currentPlayerSeat].occupant;
    if (!p) {
        POKER_LOG(LOG_INFO, "Seat %d has null occupant!", currentPlayerSeat);
        currentPlayerSeat = NextActiveSeat(currentPlayerSeat);
        return;
    }
    
    // Skip if folded
    if (statusList[currentPlayerSeat] == -1) {
        currentPlayerSeat = NextActiveSeat(currentPlayerSeat);
        return;
    }
    
    // Re-check seat is still occupied (person might have been killed/removed mid-frame)
    if (!seats[currentPlayerSeat].isOccupied || seats[currentPlayerSeat].occupant != p) {
        POKER_LOG(LOG_INFO, "Seat %d person was removed during betting!", currentPlayerSeat);
        statusList[currentPlayerSeat] = -1;  // Mark as folded
        currentPlayerSeat = NextActiveSeat(currentPlayerSeat);
        return;
    }
    
    // Calculate call amount
    int callAmount = currentBet - statusList[currentPlayerSeat];
    int chips = CountChips(p);
    int minRaise = currentBet + BIG_BLIND_AMOUNT;
    int maxRaise = statusList[currentPlayerSeat] + chips;
    int raiseAmount = 0;
    
    // Check if player has already raised this round
    bool canRaise = !hasRaised[currentPlayerSeat];
    
    // Only log when we first move to this player (prevent spamming while they think)
    if (lastLoggedPlayerSeat != currentPlayerSeat) {
        std::string personName = p->GetName();
        POKER_LOG(LOG_INFO, "%s to act: currentBet=%d, statusList[%d]=%d, callAmount=%d, canRaise=%d", 
                          personName.c_str(), currentBet, currentPlayerSeat, statusList[currentPlayerSeat], callAmount, canRaise);
        lastLoggedPlayerSeat = currentPlayerSeat;
    }
    
    // If player can't raise, force them to only fold or call
    if (!canRaise) {
        minRaise = maxRaise + 1;  // Make raise impossible
    }
    
    // Re-verify person pointer is still valid before calling PromptBet
    if (!seats[currentPlayerSeat].isOccupied || seats[currentPlayerSeat].occupant != p) {
        POKER_LOG(LOG_INFO, "ERROR: Person became invalid before PromptBet!");
        statusList[currentPlayerSeat] = -1;
        currentPlayerSeat = NextActiveSeat(currentPlayerSeat);
        return;
    }
    
    std::string personName = p->GetName();
    POKER_LOG(LOG_INFO, "About to call PromptBet on %s (ptr=%p)", personName.c_str(), (void*)p);
    int action = p->PromptBet(currentBet, callAmount, minRaise, maxRaise, raiseAmount);
    POKER_LOG(LOG_INFO, "PromptBet returned %d", action);
    
    if (action == -1) return;  // Still thinking
    
    // Process action
    if (action == 0) {
        // Fold
        POKER_LOG(LOG_INFO, "%s folds", personName.c_str());
        statusList[currentPlayerSeat] = -1;
    } else if (action == 1) {
        // Call
        POKER_LOG(LOG_INFO, "%s calls %d", personName.c_str(), callAmount);
        TakeChips(p, callAmount);
        statusList[currentPlayerSeat] += callAmount;
    } else if (action == 2) {
        // Raise (only if they haven't raised yet)
        if (canRaise) {
            int totalBet = raiseAmount - statusList[currentPlayerSeat];
            TakeChips(p, totalBet);
            statusList[currentPlayerSeat] = raiseAmount;
            currentBet = raiseAmount;
            hasRaised[currentPlayerSeat] = true;  // Mark that they've raised
            // POKER_LOG(LOG_INFO, "%s raises to %d", p->GetName().c_str(), raiseAmount);
        } else {
            // Shouldn't happen, but if it does, treat as call
            // POKER_LOG(LOG_INFO, "%s tried to raise but already raised, calling instead", p->GetName().c_str());
            TakeChips(p, callAmount);
            statusList[currentPlayerSeat] += callAmount;
        }
    }
    
    // Check if betting round is complete
    if (IsBettingRoundComplete()) {
        // POKER_LOG(LOG_INFO, "Betting round complete! communityCards.size()=%zu", communityCards.size());
        bettingActive = false;
        lastLoggedPlayerSeat = -1;  // Reset for next betting round
        
        // Count active players
        int activePlayers = 0;
        for (int i = 0; i < MAX_SEATS; i++) {
            if (seats[i].isOccupied && statusList[i] != -1) activePlayers++;
        }
        
        // POKER_LOG(LOG_INFO, "Active players: %d", activePlayers);
        
        if (activePlayers <= 1) {
            // Everyone folded, end hand
            EndHand();
        } else {
            // Progress to next street
            if (communityCards.size() == 0) {
                // POKER_LOG(LOG_INFO, "About to deal flop...");
                DealFlop();
            } else if (communityCards.size() == 3) {
                // POKER_LOG(LOG_INFO, "About to deal turn...");
                DealTurn();
            } else if (communityCards.size() == 4) {
                // POKER_LOG(LOG_INFO, "About to deal river...");
                DealRiver();
            } else {
                // Showdown - call once, then showdownActive will keep it running
                showdownActive = true;
                Showdown();
            }
        }
    } else {
        currentPlayerSeat = NextActiveSeat(currentPlayerSeat);
        // Don't reset lastLoggedPlayerSeat here - it will be updated on next ProcessBetting call
    }
}

// ========== GAME FLOW ==========

void PokerTable::StartHand() {
    if (GetOccupiedSeatCount() < 2) return;
    
    POKER_LOG(LOG_INFO, "StartHand: Beginning new hand");
    
    handActive = true;
    potValue = 0;
    currentBet = 0;
    
    // Community cards should already be cleared by EndHand()
    // Don't clear here - EndHand() needs to remove them from DOM first
    
    // Clear pot
    std::vector<Chip*> oldChips = potStack->RemoveAll();
    for (Chip* chip : oldChips) {
        delete chip;  // Delete the chips from previous hand
    }
    
    // Reset status list for all seats
    for (int i = 0; i < MAX_SEATS; i++) {
        statusList[i] = 0;
    }
    
    // Reset deck
    POKER_LOG(LOG_INFO, "StartHand: Resetting deck");
    deck->Reset();
    deck->Shuffle();
    
    // Deal hole cards
    POKER_LOG(LOG_INFO, "StartHand: About to deal hole cards");
    DealHoleCards();
    POKER_LOG(LOG_INFO, "StartHand: Hole cards dealt");
    
    // Post blinds (this sets smallBlindSeat and bigBlindSeat)
    POKER_LOG(LOG_INFO, "StartHand: About to post blinds");
    PostBlinds();
    POKER_LOG(LOG_INFO, "StartHand: Blinds posted");
    
    // Start betting (left of big blind) - don't reset bets, blinds are already posted
    currentPlayerSeat = NextOccupiedSeat(bigBlindSeat);
    bettingActive = true;
    
    POKER_LOG(LOG_INFO, "=== Hand started ===");
}

void PokerTable::DealHoleCards() {
    POKER_LOG(LOG_INFO, "DealHoleCards: Starting to deal cards");
    
    // Clear previous hand's dealt cards tracking
    for (int i = 0; i < MAX_SEATS; i++) {
        dealtHoleCards[i].clear();
    }
    
    for (int round = 0; round < 2; round++) {
        POKER_LOG(LOG_INFO, "DealHoleCards: Round %d", round);
        for (int i = 0; i < MAX_SEATS; i++) {
            if (!seats[i].isOccupied) continue;  // Skip empty seats
            
            POKER_LOG(LOG_INFO, "DealHoleCards: Checking seat %d (occupied)", i);
            
            // Safety check - verify occupant pointer is valid
            if (!seats[i].occupant) {
                POKER_LOG(LOG_INFO, "ERROR: Seat %d marked occupied but has null occupant!", i);
                seats[i].isOccupied = false;
                continue;
            }
            
            POKER_LOG(LOG_INFO, "DealHoleCards: Seat %d occupant ptr=%p, about to get type", i, (void*)seats[i].occupant);
            
            // Verify object is valid by checking type first
            std::string personType = seats[i].occupant->GetType();
            if (personType.empty()) {
                POKER_LOG(LOG_INFO, "ERROR: Seat %d occupant has null/empty type!", i);
                seats[i].isOccupied = false;
                continue;
            }
            POKER_LOG(LOG_INFO, "DealHoleCards: Seat %d occupant type=%s", i, personType.c_str());
            
            POKER_LOG(LOG_INFO, "DealHoleCards: About to get name");
            // Get name safely - store as string and use it immediately
            std::string playerName = seats[i].occupant->GetName();
            POKER_LOG(LOG_INFO, "DealHoleCards: Dealing to %s at seat %d", playerName.c_str(), i);
            
            POKER_LOG(LOG_INFO, "DealHoleCards: About to draw card from deck");
            Card* card = deck->DrawCard();
            POKER_LOG(LOG_INFO, "DealHoleCards: Drew card %p", (void*)card);
            
            if (!card) {
                POKER_LOG(LOG_INFO, "DealHoleCards: No card available from deck");
                continue;
            }
            
            POKER_LOG(LOG_INFO, "DealHoleCards: Setting card properties");
            card->canInteract = false;  // Disable interaction for hole cards
            
            POKER_LOG(LOG_INFO, "DealHoleCards: Tracking card for seat %d", i);
            // Track this card as dealt this hand
            dealtHoleCards[i].push_back(card);
            
            POKER_LOG(LOG_INFO, "DealHoleCards: Getting inventory");
            // Get inventory with safety checks
            Person* occupant = seats[i].occupant;
            if (!occupant) {
                POKER_LOG(LOG_INFO, "ERROR: Occupant became null during card dealing!");
                continue;
            }
            
            Inventory* inv = occupant->GetInventory();
            if (!inv) {
                POKER_LOG(LOG_INFO, "ERROR: Seat %d occupant has null inventory!", i);
                continue;
            }
            
            POKER_LOG(LOG_INFO, "DealHoleCards: Adding card to inventory");
            // Add card to inventory
            inv->AddItem(card);
            POKER_LOG(LOG_INFO, "DealHoleCards: Card added successfully");
        }
    }
    POKER_LOG(LOG_INFO, "DealHoleCards: Finished dealing all cards");
}

void PokerTable::PostBlinds() {
    // If this is first hand, start from seat 0, otherwise rotate from previous small blind
    if (smallBlindSeat == -1) {
        smallBlindSeat = NextOccupiedSeat(-1);  // Will find first occupied seat
    } else {
        smallBlindSeat = NextOccupiedSeat(smallBlindSeat);  // Rotate to next
    }
    bigBlindSeat = NextOccupiedSeat(smallBlindSeat);
    
    // POKER_LOG(LOG_INFO, "PostBlinds: smallBlindSeat=%d, bigBlindSeat=%d", smallBlindSeat, bigBlindSeat);
    
    // Small blind
    TakeChips(seats[smallBlindSeat].occupant, SMALL_BLIND_AMOUNT);
    statusList[smallBlindSeat] = SMALL_BLIND_AMOUNT;
    
    // Big blind
    TakeChips(seats[bigBlindSeat].occupant, BIG_BLIND_AMOUNT);
    statusList[bigBlindSeat] = BIG_BLIND_AMOUNT;
    currentBet = BIG_BLIND_AMOUNT;
    
    // POKER_LOG(LOG_INFO, "Blinds posted: seat %d (SB=%d), seat %d (BB=%d)", smallBlindSeat, SMALL_BLIND_AMOUNT, bigBlindSeat, BIG_BLIND_AMOUNT);
    // POKER_LOG(LOG_INFO, "After PostBlinds - statusList: [0]=%d [1]=%d [2]=%d [3]=%d [4]=%d [5]=%d [6]=%d [7]=%d", 
    //           statusList[0], statusList[1], statusList[2], statusList[3], 
    //           statusList[4], statusList[5], statusList[6], statusList[7]);
}

void PokerTable::StartBettingRound(int startPlayer) {
    // Reset round bets (but keep people folded as folded)
    for (int i = 0; i < MAX_SEATS; i++) {
        if (statusList[i] != -1) {
            statusList[i] = 0;
        }
        hasRaised[i] = false;  // Reset raise tracking for new betting round
    }
    currentBet = 0;
    currentPlayerSeat = startPlayer;
    bettingActive = true;
    lastLoggedPlayerSeat = -1;  // Reset logging state for new betting round
}

void PokerTable::DealFlop() {
    // Position cards in a row on the table
    float cardSpacing = 0.65f;  // Spacing between cards
    float startX = position.x - 1.0f;  // Start left of center
    float cardY = position.y + size.y/2.0f + 0.02f;  // Flush on table surface
    float cardZ = position.z + 0.5f;  // Offset to opposite side from deck/pot
    
    // POKER_LOG(LOG_INFO, "Dealing flop - cardY=%.2f, cardZ=%.2f", cardY, cardZ);
    
    for (int i = 0; i < 3; i++) {
        Card* card = deck->DrawCard();
        if (!card) {
            // POKER_LOG(LOG_INFO, "Failed to draw card %d for flop", i);
            continue;
        }
        
        card->position = {startX + (i * cardSpacing), cardY, cardZ};
        card->rotation = {-90, 0, 0};  // Lay flat on table, face up
        card->canInteract = false;  // Disable interaction for community cards
        
        // Add to DOM for rendering
        DOM::GetGlobal()->AddObject(card);
        communityCards.push_back(card);
        
        // POKER_LOG(LOG_INFO, "Flop card %d: %s at (%.2f, %.2f, %.2f)", 
    //                   i, card->GetType(), card->position.x, card->position.y, card->position.z);
    }
    
    // POKER_LOG(LOG_INFO, "Dealt flop - %zu community cards total", communityCards.size());
    StartBettingRound(NextOccupiedSeat(smallBlindSeat));  // Start from first seat after rotation
}

void PokerTable::DealTurn() {
    Card* card = deck->DrawCard();
    if (!card) return;
    
    // Continue the line of community cards (4th card)
    float cardSpacing = 0.65f;
    float startX = position.x - 1.0f;
    float cardY = position.y + size.y/2.0f + 0.02f;  // Flush on table surface
    float cardZ = position.z + 0.5f;  // Offset to opposite side from deck/pot
    
    card->position = {startX + (3 * cardSpacing), cardY, cardZ};
    card->rotation = {-90, 0, 0};  // Lay flat on table, face up
    card->canInteract = false;  // Disable interaction for community cards
    
    // Add to DOM for rendering
    DOM::GetGlobal()->AddObject(card);
    communityCards.push_back(card);
    
    // POKER_LOG(LOG_INFO, "Dealt turn");
    StartBettingRound(NextOccupiedSeat(smallBlindSeat));  // Start from first seat after rotation
}

void PokerTable::DealRiver() {
    Card* card = deck->DrawCard();
    if (!card) return;
    
    // Continue the line of community cards (5th card)
    float cardSpacing = 0.65f;
    float startX = position.x - 1.0f;
    float cardY = position.y + size.y/2.0f + 0.02f;  // Flush on table surface
    float cardZ = position.z + 0.5f;  // Offset to opposite side from deck/pot
    
    card->position = {startX + (4 * cardSpacing), cardY, cardZ};
    card->rotation = {-90, 0, 0};  // Lay flat on table, face up
    card->canInteract = false;  // Disable interaction for community cards
    
    // Add to DOM for rendering
    DOM::GetGlobal()->AddObject(card);
    communityCards.push_back(card);
    
    TraceLog(LOG_INFO, "Dealt river");
    StartBettingRound(NextOccupiedSeat(smallBlindSeat));  // Start from first seat after rotation
}

void PokerTable::Showdown() {
    TraceLog(LOG_INFO, "=== Showdown ===");
    
    // First, check if any players have 3+ cards and need to select
    for (int i = 0; i < MAX_SEATS; i++) {
        if (!seats[i].isOccupied || statusList[i] == -1) continue;
        if (!seats[i].occupant) continue;
        
        // Check if this is a player (not Enemy/Dealer) and count cards
        if (seats[i].occupant->GetType() == "player") {
            Player* player = static_cast<Player*>(seats[i].occupant);
            Inventory* inv = player->GetInventory();
            if (!inv) continue;
            
            // Count cards in inventory
            int cardCount = 0;
            for (int j = 0; j < inv->GetStackCount(); j++) {
                ItemStack* stack = inv->GetStack(j);
                if (stack && stack->item && stack->item->GetType().substr(0, 5) == "card_") {
                    cardCount++;
                }
            }
            
            // If player has 3+ cards, show selection UI
            if (cardCount >= 3) {
                // Activate card selection UI if not already active
                if (!player->cardSelectionUIActive && player->selectedCardIndices.empty()) {
                    player->cardSelectionUIActive = true;
                    showdownActive = true;  // Keep calling Showdown() until selection is done
                    POKER_LOG(LOG_INFO, "Player has %d cards - activating card selection UI", cardCount);
                }
                
                // Wait for player to complete selection
                if (player->cardSelectionUIActive) {
                    return;  // Don't proceed with showdown yet, will be called again next frame
                }
            }
        }
    }
    
    // All players have made their selections (or don't need to)
    showdownActive = false;
    
    // All players have made their selections (or don't need to), now evaluate
    int winnerIndex = -1;
    HandEvaluation bestHand = {HIGH_CARD, {}};
    
    for (int i = 0; i < MAX_SEATS; i++) {
        if (!seats[i].isOccupied || statusList[i] == -1) continue;  // Skip empty or folded
        
        // Safety check - verify occupant pointer is valid
        if (!seats[i].occupant) {
            POKER_LOG(LOG_INFO, "ERROR: Seat %d marked occupied but has null occupant in Showdown!", i);
            seats[i].isOccupied = false;
            continue;
        }
        
        HandEvaluation hand = EvaluateHand(seats[i].occupant);
        
        if (winnerIndex == -1 || CompareHands(hand, bestHand) > 0) {
            bestHand = hand;
            winnerIndex = i;
        }
    }
    
    if (winnerIndex != -1 && seats[winnerIndex].occupant) {
        std::string winnerName = seats[winnerIndex].occupant->GetName();
        TraceLog(LOG_INFO, "*** %s wins with hand rank %d! ***", winnerName.c_str(), bestHand.rank);
        GiveChips(seats[winnerIndex].occupant, potValue);
    }
    
    EndHand();
}

void PokerTable::EndHand() {
    TraceLog(LOG_INFO, "=== Hand ended ===");
    
    // If only one player left (everyone else folded), award pot
    if (bettingActive) {
        for (int i = 0; i < MAX_SEATS; i++) {
            if (seats[i].isOccupied && statusList[i] != -1) {
                // Safety check - verify occupant pointer is valid
                if (!seats[i].occupant) {
                    POKER_LOG(LOG_INFO, "ERROR: Seat %d marked occupied but has null occupant when awarding pot!", i);
                    seats[i].isOccupied = false;
                    continue;
                }
                TraceLog(LOG_INFO, "*** %s wins (others folded)! ***", seats[i].occupant->GetName().c_str());
                GiveChips(seats[i].occupant, potValue);
                break;
            }
        }
    }
    
    // Clear hole cards from all players' inventories and reset card selection
    for (int i = 0; i < MAX_SEATS; i++) {
        if (!seats[i].isOccupied) continue;
        
        // Safety check - verify occupant pointer is valid
        if (!seats[i].occupant) {
            POKER_LOG(LOG_INFO, "ERROR: Seat %d marked occupied but has null occupant in EndHand!", i);
            seats[i].isOccupied = false;
            continue;
        }
        
        // Reset card selection for players
        if (seats[i].occupant->GetType() == "player") {
            Player* player = static_cast<Player*>(seats[i].occupant);
            player->cardSelectionUIActive = false;
            player->selectedCardIndices.clear();
        }
        
        Inventory* inv = seats[i].occupant->GetInventory();
        if (!inv) {
            POKER_LOG(LOG_INFO, "ERROR: Seat %d occupant has null inventory in EndHand!", i);
            continue;
        }
        
        // Only remove the specific cards that were dealt this hand
        POKER_LOG(LOG_INFO, "EndHand: Removing %d dealt cards from seat %d", (int)dealtHoleCards[i].size(), i);
        for (Card* dealtCard : dealtHoleCards[i]) {
            // Find and remove this specific card from inventory
            for (int j = inv->GetStackCount() - 1; j >= 0; j--) {
                ItemStack* stack = inv->GetStack(j);
                if (stack && stack->item == dealtCard) {
                    // DON'T delete the card - the Deck owns it and will reuse it
                    // Just remove from inventory
                    POKER_LOG(LOG_INFO, "EndHand: Removing dealt card %s from inventory slot %d", dealtCard->GetType().c_str(), j);
                    inv->RemoveItem(j);
                    break;  // Found and removed, move to next dealt card
                }
            }
        }
        
        // Clear this seat's dealt cards list
        dealtHoleCards[i].clear();
    }
    
    // Clear community cards from DOM
    // DON'T delete these cards - the Deck owns them and will reuse them
    for (Card* card : communityCards) {
        DOM::GetGlobal()->RemoveObject(card);  // Remove from DOM but don't delete
    }
    communityCards.clear();
    
    // Clear pot chips
    std::vector<Chip*> potChips = potStack->RemoveAll();
    for (Chip* chip : potChips) {
        delete chip;  // Just delete - they were never in DOM (managed by ChipStack)
    }
    
    // Blinds will rotate on next hand (handled in PostBlinds)
    
    handActive = false;
    bettingActive = false;
    
    TraceLog(LOG_INFO, "Hand ended, blinds will rotate next hand");
}

// ========== HAND EVALUATION ==========

HandEvaluation PokerTable::EvaluateHand(Person* p) {
    // Get player's hole cards
    std::vector<Card*> allCards;
    
    if (!p) {
        return {HIGH_CARD, {}};
    }
    
    Inventory* inv = p->GetInventory();
    if (!inv) {
        return {HIGH_CARD, {}};
    }
    
    // Check if this is a Player (human) who might have selected specific cards
    if (p->GetType() == "player") {
        Player* player = static_cast<Player*>(p);
        std::vector<Card*> selectedCards = player->GetSelectedCards();
        
        // Use selected cards if available (should be exactly 2 for cheating)
        if (selectedCards.size() == 2) {
            POKER_LOG(LOG_INFO, "Using player's selected cards for evaluation (cheating)");
            allCards = selectedCards;
        } else {
            // Fallback to all cards in inventory
            for (int i = 0; i < inv->GetStackCount(); i++) {
                ItemStack* stack = inv->GetStack(i);
                if (stack && stack->item && stack->item->GetType().substr(0, 5) == "card_") {
                    Card* card = static_cast<Card*>(stack->item);
                    allCards.push_back(card);
                }
            }
        }
    } else {
        // For non-players (Enemy, Dealer), use all cards in inventory
        for (int i = 0; i < inv->GetStackCount(); i++) {
            ItemStack* stack = inv->GetStack(i);
            if (stack && stack->item && stack->item->GetType().substr(0, 5) == "card_") {
                Card* card = static_cast<Card*>(stack->item);
                allCards.push_back(card);
            }
        }
    }
    
    // Add community cards
    for (Card* card : communityCards) {
        allCards.push_back(card);
    }
    
    // For now, simple evaluation (just count rank frequencies)
    // TODO: Implement full poker hand evaluation
    
    std::map<Rank, int> rankCounts;
    std::map<Suit, int> suitCounts;
    
    for (Card* card : allCards) {
        rankCounts[card->rank]++;
        suitCounts[card->suit]++;
    }
    
    // Check for flush
    bool hasFlush = false;
    for (auto& pair : suitCounts) {
        if (pair.second >= 5) hasFlush = true;
    }
    
    // Check for pairs, trips, quads
    int pairs = 0;
    int trips = 0;
    int quads = 0;
    Rank highestPairRank = RANK_TWO;
    
    for (auto& pair : rankCounts) {
        if (pair.second == 4) quads++;
        if (pair.second == 3) trips++;
        if (pair.second == 2) {
            pairs++;
            if (pair.first > highestPairRank) highestPairRank = pair.first;
        }
    }
    
    HandEvaluation result;
    
    if (quads > 0) {
        result.rank = FOUR_OF_KIND;
    } else if (trips > 0 && pairs > 0) {
        result.rank = FULL_HOUSE;
    } else if (hasFlush) {
        result.rank = FLUSH;
    } else if (trips > 0) {
        result.rank = THREE_OF_KIND;
    } else if (pairs >= 2) {
        result.rank = TWO_PAIR;
    } else if (pairs == 1) {
        result.rank = PAIR;
        result.rankValues.push_back(highestPairRank);
    } else {
        result.rank = HIGH_CARD;
    }
    
    return result;
}

int PokerTable::CompareHands(const HandEvaluation& h1, const HandEvaluation& h2) {
    if (h1.rank > h2.rank) return 1;
    if (h1.rank < h2.rank) return -1;
    
    // Same rank, compare kickers
    for (size_t i = 0; i < h1.rankValues.size() && i < h2.rankValues.size(); i++) {
        if (h1.rankValues[i] > h2.rankValues[i]) return 1;
        if (h1.rankValues[i] < h2.rankValues[i]) return -1;
    }
    
    return 0;  // Tie
}
