#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h"
#include "entities/person.hpp"
#include "rendering/camera.hpp"
#include "core/physics.hpp"
#include "gameplay/insanity_manager.hpp"
#include <ode/ode.h>
#include <vector>

// Forward declarations
class Interactable;
class Card;

// Collision categories
#define COLLISION_CATEGORY_PLAYER   (1 << 0)  // 0001
#define COLLISION_CATEGORY_ITEM     (1 << 1)  // 0010
#define COLLISION_CATEGORY_TABLE    (1 << 2)  // 0100

class Player : public Person {
private:
    GameCamera camera;
    float speed;
    float lookYaw;
    float lookPitch;

    // Physics
    dBodyID body;
    dGeomID geom;
    PhysicsWorld* physics;

    // Inventory selection
    int selectedItemIndex;  // -1 = no item selected, 0+ = selected item index
    int lastHeldItemIndex;  // Remembers the last item that was held
    
    // Betting UI state
    bool bettingUIActive;   // Is betting UI currently shown
    int bettingChoice;      // -1=waiting, 0=fold, 1=call, 2=raise
    int raiseSliderValue;   // Current raise amount from slider
    int raiseMin;           // Minimum raise
    int raiseMax;           // Maximum raise
    int storedCurrentBet;   // Stored for UI display
    int storedCallAmount;   // Stored for UI display

public:
    // Card selection UI state (for cheating with 3+ cards) - public so poker table can access
    bool cardSelectionUIActive;     // Is card selection UI shown
    std::vector<int> selectedCardIndices;  // Indices of selected cards (max 2)
    
    // Insanity system - public so main.cpp can access DrawMeter
    InsanityManager insanityManager;
    Player(Vector3 pos, PhysicsWorld* physicsWorld, const std::string& playerName = "Player");
    virtual ~Player();

    // Override virtual functions
    void Update(float deltaTime) override;
    std::string GetType() const override;

    // Player-specific methods
    void HandleInteraction();
    void HandleUseItem();
    Interactable* GetClosestInteractable();
    void Draw(Camera3D camera) override;
    void DrawInventoryUI();
    void DrawHeldItem();

    // Seating methods (override to handle physics)
    void SitDown(Vector3 seatPos) override;
    void StandUp() override;
    
    // Override PromptBet for UI-based betting
    int PromptBet(int currentBet, int callAmount, int minRaise, int maxRaise, int& raiseAmount) override;
    
    // Betting UI
    void DrawBettingUI();
    
    // Card selection UI (for cheating with 3+ cards)
    void DrawCardSelectionUI();
    std::vector<Card*> GetSelectedCards();  // Returns the 2 selected cards for hand evaluation
    
    // Insanity management
    void OnKillPerson();  // Called when player kills someone
    float GetInsanity() const;  // Get current insanity level
    bool IsDead() const;  // Check if player has died from insanity
    
    // Accessors
    Camera3D* GetCamera() { return camera.GetCamera(); }
    Inventory* GetInventory() { return &inventory; }
    Vector3 GetPosition() const { return position; }
    int GetSelectedItemIndex() const { return selectedItemIndex; }
};

#endif
