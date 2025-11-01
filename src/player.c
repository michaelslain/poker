#include "player.h"
#include "poker_table.h"
#include "item.h"
#include "dom.h"
#include "raymath.h"
#include <math.h>
#include <string.h>
#include <ode/ode.h>

const char* Player_GetType(Object* obj) {
    (void)obj;
    return "player";
}

void Player_Init(Player* player, Vector3 pos, PhysicsWorld* physics) {
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
    
    // Initialize physics
    player->physics = physics;
    
    if (physics != NULL) {
        // Create kinematic body (controlled by code, not physics forces)
        player->body = dBodyCreate(physics->world);
        dBodySetPosition(player->body, pos.x, pos.y + 0.85f, pos.z);  // Center at mid-height
        
        // Set kinematic flag (body won't be affected by forces/gravity)
        dBodySetKinematic(player->body);
        
        // Create capsule geometry (bean-shaped)
        // Capsule parameters: radius, length (of cylindrical section)
        float radius = 0.4f;  // Not too wide
        float height = 1.7f;  // Camera height
        float cylinderLength = height - (2.0f * radius);  // Subtract the two hemisphere caps
        
        player->geom = dCreateCapsule(physics->space, radius, cylinderLength);
        dGeomSetBody(player->geom, player->body);
        
        // Set collision category and mask
        // Player collides with TABLE only (not with ITEM)
        dGeomSetCategoryBits(player->geom, COLLISION_CATEGORY_PLAYER);
        dGeomSetCollideBits(player->geom, COLLISION_CATEGORY_TABLE);
        
        // Store reference to player for collision callbacks
        dGeomSetData(player->geom, player);
    } else {
        player->body = NULL;
        player->geom = NULL;
    }
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
        
        Vector3 oldPos = player->base.position;
        Vector3 newPos = Vector3Add(player->base.position, moveDir);
        
        // Update physics body position and check for collisions
        if (player->body != NULL && player->geom != NULL) {
            // Temporarily move the player geometry to the new position
            dGeomSetPosition(player->geom, newPos.x, newPos.y + 0.85f, newPos.z);
            
            // Check for collisions with the poker table
            bool collided = false;
            DOM* dom = DOM_GetGlobal();
            if (dom) {
                for (int i = 0; i < dom->count; i++) {
                    Object* obj = dom->objects[i];
                    if (obj->getType == NULL) continue;
                    
                    const char* typeStr = obj->getType(obj);
                    if (strcmp(typeStr, "poker_table") == 0) {
                        PokerTable* table = (PokerTable*)obj;
                        
                        if (table->geom != NULL) {
                            // Check collision between player geom and table geom
                            dContactGeom contacts[4];
                            int numContacts = dCollide(player->geom, table->geom, 4, contacts, sizeof(dContactGeom));
                            
                            if (numContacts > 0) {
                                collided = true;
                                break;
                            }
                        }
                    }
                }
            }
            
            if (collided) {
                // Revert to old position
                dGeomSetPosition(player->geom, oldPos.x, oldPos.y + 0.85f, oldPos.z);
                dBodySetPosition(player->body, oldPos.x, oldPos.y + 0.85f, oldPos.z);
            } else {
                // Accept new position
                dBodySetPosition(player->body, newPos.x, newPos.y + 0.85f, newPos.z);
                player->base.position = newPos;
            }
        } else {
            // No physics - just move directly
            player->base.position = newPos;
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

// OLD DISTANCE-BASED SYSTEM (COMMENTED OUT)
// Interactable* Player_GetClosestInteractable(Player* player) {
//     // Get global DOM instance
//     DOM* dom = DOM_GetGlobal();
//     if (!dom) return NULL;
//     
//     // Find closest interactable in DOM
//     Interactable* closestInteractable = NULL;
//     float closestDistance = 999999.0f;
//     Vector3 playerPos = player->base.position;
//     
//     for (int i = 0; i < dom->count; i++) {
//         Object* obj = dom->objects[i];
//         
//         // Skip if no type function
//         if (obj->getType == NULL) continue;
//         const char* typeStr = obj->getType(obj);
//         
//         // Check for items (cards, chips) or poker table
//         bool isItem = (strncmp(typeStr, "card_", 5) == 0 || strncmp(typeStr, "chip_", 5) == 0);
//         bool isPokerTable = (strcmp(typeStr, "poker_table") == 0);
//         
//         if (!isItem && !isPokerTable) continue;
//         
//         // Cast to Interactable based on type
//         Interactable* interactable = NULL;
//         if (isItem) {
//             interactable = &((Item*)obj)->base;
//         } else if (isPokerTable) {
//             interactable = &((PokerTable*)obj)->base;
//         }
//         
//         if (!interactable->isActive) continue;
//         float dist = Vector3Distance(playerPos, interactable->base.position);
//         if (dist <= interactable->interactRange && dist < closestDistance) {
//             closestDistance = dist;
//             closestInteractable = interactable;
//         }
//     }
//     
//     return closestInteractable;
// }

// NEW CROSSHAIR-BASED SYSTEM
Interactable* Player_GetClosestInteractable(Player* player) {
    // Get global DOM instance
    DOM* dom = DOM_GetGlobal();
    if (!dom) return NULL;
    
    // Get camera for raycasting
    Camera3D* camera = Player_GetCamera(player);
    
    // Create a ray from the camera center (screen center)
    Vector3 rayOrigin = camera->position;
    Vector3 rayDirection = Vector3Normalize((Vector3){
        camera->target.x - camera->position.x,
        camera->target.y - camera->position.y,
        camera->target.z - camera->position.z
    });
    
    // Find the interactable closest to the center of the camera view
    Interactable* closestInteractable = NULL;
    float closestDistance = 999999.0f;
    float maxInteractDistance = 5.0f;  // Max interaction range
    
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
        
        // Calculate distance from ray to object center
        Vector3 objPos = interactable->base.position;
        Vector3 toObj = Vector3Subtract(objPos, rayOrigin);
        
        // Project point onto ray
        float projection = Vector3DotProduct(toObj, rayDirection);
        
        // Object must be in front of camera and within range
        if (projection < 0.1f || projection > maxInteractDistance) continue;
        
        // Find closest point on ray to object
        Vector3 closestPointOnRay = Vector3Add(rayOrigin, Vector3Scale(rayDirection, projection));
        
        // Calculate distance from object to ray (how close to crosshair center)
        float distanceToRay = Vector3Distance(objPos, closestPointOnRay);
        
        // Use a threshold for "close enough to crosshair"
        float crosshairThreshold = 1.0f;  // Objects within 1 unit of ray center
        
        if (distanceToRay < crosshairThreshold && projection < closestDistance) {
            closestDistance = projection;
            closestInteractable = interactable;
        }
    }
    
    return closestInteractable;
}

void Player_Cleanup(Player* player) {
    // Destroy physics geometry and body
    if (player->geom != NULL) {
        dGeomDestroy(player->geom);
        player->geom = NULL;
    }
    if (player->body != NULL) {
        dBodyDestroy(player->body);
        player->body = NULL;
    }
    
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
