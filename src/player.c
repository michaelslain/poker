#include "player.h"
#include "poker_table.h"
#include "item.h"
#include "dom.h"
#include "raymath.h"
#include <math.h>
#include <string.h>

const char* Player_GetType(Object* obj) {
    (void)obj;
    return "player";
}

void Player_Init(Player* player, Vector3 pos) {
    Object_Init(&player->base, pos);
    
    // Set the getType function pointer
    player->base.getType = Player_GetType;
    
    // Initialize camera at eye level
    Vector3 eyePos = pos;
    eyePos.y += 1.7f;
    GameCamera_Init(&player->camera, eyePos);
    
    // Initialize inventory with capacity of 20
    Inventory_Init(&player->inventory, 20);
    
    player->speed = 5.0f;
    player->lookYaw = 0.0f;
    player->lookPitch = 0.0f;
}

void Player_HandleInteraction(Player* player) {
    if (!IsKeyPressed(KEY_E)) return;
    
    // Use the existing function to find closest interactable
    Interactable* closestInteractable = Player_GetClosestInteractable(player);
    if (!closestInteractable) return;
    
    // Handle interaction based on type
    const char* typeStr = closestInteractable->base.getType((Object*)closestInteractable);
    
    // Handle poker table interaction
    if (strcmp(typeStr, "poker_table") == 0) {
        PokerTable_InteractWithPlayer((PokerTable*)closestInteractable, player);
    }
    // Handle item pickup
    else if (strncmp(typeStr, "card_", 5) == 0 || strncmp(typeStr, "chip_", 5) == 0) {
        Item* item = (Item*)closestInteractable;
        
        // Add to inventory and deactivate
        Inventory_AddItem(&player->inventory, item);
        item->base.isActive = false;
        
        TraceLog(LOG_INFO, "Item picked up: %s, Inventory stacks: %d", typeStr, player->inventory.stackCount);
    }
}

void Player_Update(Player* player, float deltaTime) {
    // Mouse look
    Vector2 mouseDelta = GetMouseDelta();
    
    // Update player's look angles
    float sensitivity = 0.003f;
    player->lookYaw -= mouseDelta.x * sensitivity;
    player->lookPitch -= mouseDelta.y * sensitivity;
    
    // Clamp pitch
    if (player->lookPitch > 89.0f * DEG2RAD) player->lookPitch = 89.0f * DEG2RAD;
    if (player->lookPitch < -89.0f * DEG2RAD) player->lookPitch = -89.0f * DEG2RAD;
    
    // Calculate forward and right vectors from yaw (ignore pitch for movement)
    Vector3 forward = {
        sinf(player->lookYaw),
        0.0f,
        cosf(player->lookYaw)
    };
    
    Vector3 right = {
        sinf(player->lookYaw + PI/2),
        0.0f,
        cosf(player->lookYaw + PI/2)
    };
    
    // WASD movement
    Vector3 moveDir = {0.0f, 0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W)) {
        moveDir = Vector3Add(moveDir, forward);
    }
    if (IsKeyDown(KEY_S)) {
        moveDir = Vector3Subtract(moveDir, forward);
    }
    if (IsKeyDown(KEY_D)) {
        moveDir = Vector3Subtract(moveDir, right);
    }
    if (IsKeyDown(KEY_A)) {
        moveDir = Vector3Add(moveDir, right);
    }
    
    // Normalize and apply speed
    if (Vector3Length(moveDir) > 0) {
        moveDir = Vector3Normalize(moveDir);
        moveDir = Vector3Scale(moveDir, player->speed * deltaTime);
        
        // Store old position for collision resolution
        Vector3 oldPos = player->base.position;
        Vector3 newPos = Vector3Add(player->base.position, moveDir);
        player->base.position = newPos;
        
        // Check collision with poker tables
        DOM* dom = DOM_GetGlobal();
        if (dom) {
            float playerRadius = 0.5f; // Player collision radius
            
            for (int i = 0; i < dom->count; i++) {
                Object* obj = dom->objects[i];
                if (obj->getType == NULL) continue;
                
                const char* typeStr = obj->getType(obj);
                if (strcmp(typeStr, "poker_table") == 0) {
                    PokerTable* table = (PokerTable*)obj;
                    
                    // Simple AABB collision (box vs sphere)
                    Vector3 tablePos = table->base.base.position;
                    Vector3 halfSize = {table->size.x / 2, table->size.y / 2, table->size.z / 2};
                    
                    // Find closest point on table to player
                    Vector3 closest;
                    closest.x = fmaxf(tablePos.x - halfSize.x, fminf(newPos.x, tablePos.x + halfSize.x));
                    closest.y = fmaxf(tablePos.y - halfSize.y, fminf(newPos.y, tablePos.y + halfSize.y));
                    closest.z = fmaxf(tablePos.z - halfSize.z, fminf(newPos.z, tablePos.z + halfSize.z));
                    
                    float dist = Vector3Distance(newPos, closest);
                    
                    if (dist < playerRadius) {
                        // Collision! Revert to old position
                        player->base.position = oldPos;
                        break;
                    }
                }
            }
        }
    }
    
    // FOV adjustment
    GameCamera_AdjustFOV(&player->camera);
    
    // Update camera to follow player and match look direction
    player->camera.angle.x = player->lookPitch;
    player->camera.angle.y = player->lookYaw;
    
    // Position camera at player's eye level (add 1.7 to y for eye height)
    Vector3 eyePos = player->base.position;
    eyePos.y += 1.7f;
    GameCamera_SetTarget(&player->camera, eyePos);
    GameCamera_Update(&player->camera, (Vector2){0, 0}); // Already handled mouse delta above
    
    // Handle interaction (E key)
    Player_HandleInteraction(player);
}

Interactable* Player_GetClosestInteractable(Player* player) {
    // Get global DOM instance
    DOM* dom = DOM_GetGlobal();
    if (!dom) return NULL;
    
    // Find closest interactable in DOM
    Interactable* closestInteractable = NULL;
    float closestDistance = 999999.0f;
    Vector3 playerPos = player->base.position;
    
    for (int i = 0; i < dom->count; i++) {
        Object* obj = dom->objects[i];
        
        // Skip if no type function
        if (obj->getType == NULL) continue;
        const char* typeStr = obj->getType(obj);
        
        // Check for items (cards, chips) or poker table
        bool isItem = (strncmp(typeStr, "card_", 5) == 0 || strncmp(typeStr, "chip_", 5) == 0);
        bool isPokerTable = (strcmp(typeStr, "poker_table") == 0);
        
        if (!isItem && !isPokerTable) continue;
        
        // Cast to Interactable based on type
        Interactable* interactable = NULL;
        if (isItem) {
            interactable = &((Item*)obj)->base;
        } else if (isPokerTable) {
            interactable = &((PokerTable*)obj)->base;
        }
        
        if (!interactable->isActive) continue;
        float dist = Vector3Distance(playerPos, interactable->base.position);
        if (dist <= interactable->interactRange && dist < closestDistance) {
            closestDistance = dist;
            closestInteractable = interactable;
        }
    }
    
    return closestInteractable;
}

void Player_Cleanup(Player* player) {
    Inventory_Cleanup(&player->inventory);
}

void Player_DrawInventoryUI(Player* player) {
    InventoryUI_Draw(&player->inventory);
}

Vector3 Player_GetPosition(Player* player) {
    return player->base.position;
}

Camera3D* Player_GetCamera(Player* player) {
    return &player->camera.camera;
}

Inventory* Player_GetInventory(Player* player) {
    return &player->inventory;
}
