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

class Player : public Person {
private:
    // Player-specific movement constants
    static constexpr float MOVEMENT_FORCE = 500.0f;       // Horizontal movement force
    static constexpr float VELOCITY_DAMPING = 0.8f;       // Horizontal velocity damping (0-1)
    
    GameCamera camera;
    float speed;
    float speedBoostTimer;      // Time remaining for speed boost
    float baseSpeed;            // Normal speed (5.0)
    float lookYaw;
    float lookPitch;

    // Global player instance (for substances to access insanity manager)
    static Player* globalInstance;

    // Death state
    bool isDying;                   // Death sequence started
    float deathVignetteProgress;    // 0.0 to 1.0 (fully enclosed)
    Shader vignetteShader;          // Vignette post-processing shader
    bool vignetteShaderLoaded;      // Track if shader was loaded successfully
    static constexpr float DEATH_VIGNETTE_DURATION = 3.0f;  // 3 seconds to close

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
    Player(Vector3 pos, const std::string& playerName = "Player");
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
    
    // Speed boost management
    void ApplySpeedBoost(float multiplier, float duration);  // Apply temporary speed boost
    
    // Death management
    void TriggerDeath();  // Trigger death sequence (from insanity, fent, etc.)
    bool IsDying() const { return isDying; }
    bool IsDead() const { return isDying && deathVignetteProgress >= 1.0f; }
    void DrawDeathVignette();  // Draw death vignette overlay
    
    // Teleportation (for level transitions)
    void Teleport(Vector3 newPos);
    
    // Accessors
    Camera3D* GetCamera() { return camera.GetCamera(); }
    Inventory* GetInventory() { return &inventory; }
    Vector3 GetPosition() const { return position; }
    int GetSelectedItemIndex() const { return selectedItemIndex; }
    dGeomID GetGeom() const { return geom; }

    // Global instance management (for substances to access player)
    static void SetGlobal(Player* instance);
    static Player* GetGlobal();
};

#endif
