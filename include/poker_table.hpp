#ifndef POKER_TABLE_HPP
#define POKER_TABLE_HPP

#include "interactable.hpp"
#include "deck.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "person.hpp"
#include "physics.hpp"
#include <ode/ode.h>
#include <array>
#include <vector>

#define MAX_SEATS 8
#define SMALL_BLIND_AMOUNT 5
#define BIG_BLIND_AMOUNT 10

// Poker logging control - bypasses raylib's SetTraceLogLevel
#define POKER_LOG_ENABLED true
#define POKER_LOG(level, ...) do { if (POKER_LOG_ENABLED) { printf("[POKER] "); printf(__VA_ARGS__); printf("\n"); } } while(0)

// Collision categories
#ifndef COLLISION_CATEGORY_PLAYER
#define COLLISION_CATEGORY_PLAYER   (1 << 0)
#define COLLISION_CATEGORY_TABLE    (1 << 2)
#endif

// Forward declarations
class Dealer;

// Hand rankings
enum HandRank {
    HIGH_CARD = 0,
    PAIR = 1,
    TWO_PAIR = 2,
    THREE_OF_KIND = 3,
    STRAIGHT = 4,
    FLUSH = 5,
    FULL_HOUSE = 6,
    FOUR_OF_KIND = 7,
    STRAIGHT_FLUSH = 8,
    ROYAL_FLUSH = 9
};

struct HandEvaluation {
    HandRank rank;
    std::vector<int> rankValues;  // For tie-breaking (e.g., pair of Aces > pair of Kings)
};

struct Seat {
    Vector3 position;
    Person* occupant;     // nullptr if empty
    bool isOccupied;
};

class PokerTable : public Interactable {
private:
    // Visual
    Vector3 size;
    Color color;
    dGeomID geom;
    PhysicsWorld* physics;
    
    // Game objects (dual-reference: attributes for logic, children for rendering)
    Dealer* dealer;
    Deck* deck;
    std::vector<Chip*> pot;              // Chips in the pot (also in children)
    std::vector<Card*> communityCards;   // Community cards (also in children)
    
    // Seating - fixed size array
    std::array<Seat, MAX_SEATS> seats;
    std::array<int, MAX_SEATS> statusList;  // Betting status: -1 = folded, >= 0 = current bet
    std::array<bool, MAX_SEATS> hasRaised;  // Track if player has raised this betting round
    
    // Game state
    int smallBlindSeat;     // Index of seat with small blind
    int bigBlindSeat;       // Index of seat with big blind
    int currentPlayerSeat;  // Index of seat for current player (for betting)
    int currentBet;         // Current bet to match
    int potValue;           // Total value of pot
    bool handActive;
    bool bettingActive;
    
    // Logging state (to prevent duplicate logs)
    int lastLoggedPlayerSeat;  // Last player seat that was logged
    
    // Helper functions - Chip management
    int CountChips(Person* p);
    int CountChips(std::vector<Chip*>& chips);
    void TakeChips(Person* p, int amount);
    void GiveChips(Person* p, int amount);
    std::vector<Chip*> CalculateChipCombination(int amount);
    
    // Helper functions - Seat navigation
    int NextOccupiedSeat(int index);
    int NextActiveSeat(int index);  // Next seat that's occupied and not folded
    int GetOccupiedSeatCount();
    
    // Helper functions - Betting
    bool IsBettingRoundComplete();
    void ProcessBetting(float dt);
    
    // Helper functions - Hand evaluation
    HandEvaluation EvaluateHand(Person* p);
    int CompareHands(const HandEvaluation& h1, const HandEvaluation& h2);
    
    // Game flow
    void StartHand();
    void DealHoleCards();
    void PostBlinds();
    void StartBettingRound(int startPlayer);
    void DealFlop();
    void DealTurn();
    void DealRiver();
    void Showdown();
    void EndHand();
    
public:
    PokerTable(Vector3 pos, Vector3 size, Color color, PhysicsWorld* physics);
    ~PokerTable();
    
    // Overrides
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    void Interact() override;
    const char* GetType() const override;
    
    // Seating
    int FindClosestOpenSeat(Vector3 pos);
    bool SeatPerson(Person* p, int seatIndex);
    void UnseatPerson(Person* p);
    int FindSeatIndex(Person* p);  // Returns seat index or -1 if not seated
    
    // Accessors
    dGeomID GetGeom() const { return geom; }
};

#endif
