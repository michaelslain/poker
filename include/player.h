#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "object.h"
#include "camera.h"
#include "inventory.h"
#include "inventory_ui.h"
#include "physics.h"
#include <ode/ode.h>

// Forward declaration
typedef struct Interactable Interactable;

// Collision categories
#define COLLISION_CATEGORY_PLAYER   (1 << 0)  // 0001
#define COLLISION_CATEGORY_ITEM     (1 << 1)  // 0010
#define COLLISION_CATEGORY_TABLE    (1 << 2)  // 0100

typedef struct {
    Object base;
    GameCamera camera;
    Inventory inventory;
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
} Player;

void Player_Init(Player* player, Vector3 pos, PhysicsWorld* physics);
void Player_Update(Player* player, float deltaTime);
void Player_HandleInteraction(Player* player);
Interactable* Player_GetClosestInteractable(Player* player);
void Player_Cleanup(Player* player);
void Player_DrawInventoryUI(Player* player);
Vector3 Player_GetPosition(Player* player);
Camera3D* Player_GetCamera(Player* player);
Inventory* Player_GetInventory(Player* player);

const char* Player_GetType(Object* obj);

#endif
