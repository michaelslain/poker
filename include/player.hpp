#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h"
#include "person.hpp"
#include "camera.hpp"
#include "physics.hpp"
#include <ode/ode.h>

// Forward declaration
class Interactable;

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

public:
    Player(Vector3 pos, PhysicsWorld* physicsWorld, const std::string& playerName = "Player");
    virtual ~Player();

    // Override virtual functions
    void Update(float deltaTime) override;
    const char* GetType() const override;

    // Player-specific methods
    void HandleInteraction();
    void HandleShooting();
    Interactable* GetClosestInteractable();
    void DrawInventoryUI();
    void DrawHeldItem();

    // Seating methods (override to handle physics)
    void SitDown(Vector3 seatPos) override;
    void StandUp() override;
    
    // Accessors
    Camera3D* GetCamera() { return camera.GetCamera(); }
    Inventory* GetInventory() { return &inventory; }
    Vector3 GetPosition() const { return position; }
    int GetSelectedItemIndex() const { return selectedItemIndex; }
};

#endif
