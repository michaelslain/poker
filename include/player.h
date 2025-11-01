#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "object.h"
#include "camera.h"
#include "inventory.h"
#include "inventory_ui.h"

// Forward declaration
typedef struct Interactable Interactable;

typedef struct {
    Object base;
    GameCamera camera;
    Inventory inventory;
    float speed;
    float lookYaw;
    float lookPitch;
} Player;

void Player_Init(Player* player, Vector3 pos);
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
