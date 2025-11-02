#include "player.hpp"
#include "item.hpp"
#include "interactable.hpp"
#include "poker_table.hpp"
#include "wall.hpp"
#include "pistol.hpp"
#include "bullet.hpp"
#include "dom.hpp"
#include "inventory_ui.hpp"
#include "raymath.h"
#include <cmath>
#include <cstring>
#include <ode/ode.h>

Player::Player(Vector3 pos, PhysicsWorld* physicsWorld, const std::string& playerName)
    : Person(pos, playerName, 1.0f), camera({pos.x, pos.y + 1.7f, pos.z}), speed(5.0f),
      lookYaw(0.0f), lookPitch(0.0f), body(nullptr), geom(nullptr), physics(physicsWorld),
      selectedItemIndex(-1), lastHeldItemIndex(-1)
{
    if (physics != nullptr) {
        // Create kinematic body (controlled by code, not physics forces)
        body = dBodyCreate(physics->world);
        dBodySetPosition(body, pos.x, pos.y + 0.85f, pos.z);
        dBodySetKinematic(body);

        // Create capsule geometry
        float radius = 0.4f;
        float height = 1.7f;
        float cylinderLength = height - (2.0f * radius);

        geom = dCreateCapsule(physics->space, radius, cylinderLength);
        dGeomSetBody(geom, body);

        // Set collision category and mask (collide with everything)
        dGeomSetCategoryBits(geom, COLLISION_CATEGORY_PLAYER);
        dGeomSetCollideBits(geom, ~0);  // Collide with all objects

        dGeomSetData(geom, this);
    }
}

Player::~Player() {
    if (geom != nullptr) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    if (body != nullptr) {
        dBodyDestroy(body);
        body = nullptr;
    }
}

const char* Player::GetType() const {
    return "player";
}

void Player::HandleInteraction() {
    if (!IsKeyPressed(KEY_E)) return;

    Interactable* closestInteractable = GetClosestInteractable();
    if (!closestInteractable) return;

    const char* typeStr = closestInteractable->GetType();

    // Handle poker table interaction
    if (strcmp(typeStr, "poker_table") == 0) {
        PokerTable* table = static_cast<PokerTable*>(closestInteractable);
        table->InteractWithPlayer(this);
    }
    // Handle item pickup
    else if (strncmp(typeStr, "card_", 5) == 0 || strncmp(typeStr, "chip_", 5) == 0 || strcmp(typeStr, "pistol") == 0) {
        Item* item = static_cast<Item*>(closestInteractable);

        // Add to inventory
        inventory.AddItem(item);

        // Remove from DOM - inventory now owns it
        DOM* dom = DOM::GetGlobal();
        if (dom) {
            dom->RemoveObject(item);
        }

        TraceLog(LOG_INFO, "Item picked up: %s, Inventory stacks: %d", typeStr, inventory.GetStackCount());
    }
}

void Player::Update(float deltaTime) {
    // Mouse look
    Vector2 mouseDelta = GetMouseDelta();

    float sensitivity = 0.001f;  // Reduced from 0.003f for slower mouse movement
    lookYaw -= mouseDelta.x * sensitivity;
    lookPitch -= mouseDelta.y * sensitivity;

    // Clamp pitch
    if (lookPitch > 89.0f * DEG2RAD) lookPitch = 89.0f * DEG2RAD;
    if (lookPitch < -89.0f * DEG2RAD) lookPitch = -89.0f * DEG2RAD;

    // Calculate forward and right vectors from yaw
    Vector3 forward = {
        sinf(lookYaw),
        0.0f,
        cosf(lookYaw)
    };

    Vector3 right = {
        sinf(lookYaw + PI/2),
        0.0f,
        cosf(lookYaw + PI/2)
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
        moveDir = Vector3Scale(moveDir, speed * deltaTime);

        Vector3 oldPos = position;
        Vector3 newPos = Vector3Add(position, moveDir);

        // Update physics body position and check for collisions
        if (body != nullptr && geom != nullptr) {
            dGeomSetPosition(geom, newPos.x, newPos.y + 0.85f, newPos.z);

            // Check for collisions with objects using ODE
            bool collided = false;
            DOM* dom = DOM::GetGlobal();
            if (dom) {
                for (int i = 0; i < dom->GetCount(); i++) {
                    Object* obj = dom->GetObject(i);
                    const char* typeStr = obj->GetType();

                    dGeomID otherGeom = nullptr;
                    
                    if (strcmp(typeStr, "poker_table") == 0) {
                        PokerTable* table = static_cast<PokerTable*>(obj);
                        otherGeom = table->GetGeom();
                    } else if (strcmp(typeStr, "wall") == 0) {
                        Wall* wall = static_cast<Wall*>(obj);
                        otherGeom = wall->GetGeom();
                    }

                    if (otherGeom != nullptr) {
                        // Use ODE collision detection
                        dContactGeom contacts[4];
                        int numContacts = dCollide(geom, otherGeom, 4, contacts, sizeof(dContactGeom));

                        if (numContacts > 0) {
                            collided = true;
                            break;
                        }
                    }
                }
            }

            if (collided) {
                // Revert to old position
                dGeomSetPosition(geom, oldPos.x, oldPos.y + 0.85f, oldPos.z);
                dBodySetPosition(body, oldPos.x, oldPos.y + 0.85f, oldPos.z);
            } else {
                // Accept new position
                dBodySetPosition(body, newPos.x, newPos.y + 0.85f, newPos.z);
                position = newPos;
            }
        } else {
            // No physics - just move directly
            position = newPos;
        }
    }

    // FOV adjustment
    camera.AdjustFOV();

    // Update camera to follow player
    camera.angle.x = lookPitch;
    camera.angle.y = lookYaw;

    // Position camera slightly in front of the player's face (like eyes)
    // This prevents the camera from being inside the pitch-black head
    Vector3 forwardDir = {
        sinf(lookYaw),
        0.0f,
        cosf(lookYaw)
    };
    
    Vector3 eyePos = position;
    eyePos.y += 1.9f * height;  // Eye level height (scaled by height)
    eyePos.x += forwardDir.x * 0.3f;  // Offset forward slightly
    eyePos.z += forwardDir.z * 0.3f;
    
    camera.SetTarget(eyePos);
    camera.Update({0, 0});

    // Handle inventory selection with X key
    if (IsKeyPressed(KEY_X)) {
        TraceLog(LOG_INFO, "X key pressed! Current selected: %d, Stack count: %d", selectedItemIndex, inventory.GetStackCount());
        if (selectedItemIndex == -1) {
            if (inventory.GetStackCount() > 0) {
                if (lastHeldItemIndex >= 0 && lastHeldItemIndex < inventory.GetStackCount()) {
                    selectedItemIndex = lastHeldItemIndex;
                } else {
                    selectedItemIndex = 0;
                }
                TraceLog(LOG_INFO, "Selected item index: %d", selectedItemIndex);
            }
        } else {
            lastHeldItemIndex = selectedItemIndex;
            selectedItemIndex = -1;
            TraceLog(LOG_INFO, "Deselected item. Last held: %d", lastHeldItemIndex);
        }
    }

    // Handle left/right arrow keys for inventory navigation
    if (IsKeyPressed(KEY_RIGHT)) {
        if (inventory.GetStackCount() > 0) {
            if (selectedItemIndex == -1) {
                selectedItemIndex = 0;
            } else {
                selectedItemIndex = (selectedItemIndex + 1) % inventory.GetStackCount();
            }
            lastHeldItemIndex = selectedItemIndex;
        }
    }

    if (IsKeyPressed(KEY_LEFT)) {
        if (inventory.GetStackCount() > 0) {
            if (selectedItemIndex == -1) {
                selectedItemIndex = inventory.GetStackCount() - 1;
            } else {
                selectedItemIndex--;
                if (selectedItemIndex < 0) {
                    selectedItemIndex = inventory.GetStackCount() - 1;
                }
            }
            lastHeldItemIndex = selectedItemIndex;
        }
    }

    // Clamp selected index if inventory changed
    if (inventory.GetStackCount() == 0) {
        selectedItemIndex = -1;
        lastHeldItemIndex = -1;
    } else if (selectedItemIndex >= inventory.GetStackCount()) {
        selectedItemIndex = inventory.GetStackCount() - 1;
    }

    if (lastHeldItemIndex >= inventory.GetStackCount()) {
        lastHeldItemIndex = -1;
    }

    // Handle interaction (E key)
    HandleInteraction();

    // Handle shooting (left click)
    HandleShooting();
}

Interactable* Player::GetClosestInteractable() {
    DOM* dom = DOM::GetGlobal();
    if (!dom) return nullptr;

    Camera3D* cam = GetCamera();

    // Create a ray from the camera center
    Vector3 rayOrigin = cam->position;
    Vector3 rayDirection = Vector3Normalize({
        cam->target.x - cam->position.x,
        cam->target.y - cam->position.y,
        cam->target.z - cam->position.z
    });

    Interactable* closestInteractable = nullptr;
    float closestDistance = 999999.0f;
    float maxInteractDistance = 5.0f;

    for (int i = 0; i < dom->GetCount(); i++) {
        Object* obj = dom->GetObject(i);
        const char* typeStr = obj->GetType();

        bool isItem = (strncmp(typeStr, "card_", 5) == 0 || strncmp(typeStr, "chip_", 5) == 0 || strcmp(typeStr, "pistol") == 0);
        bool isPokerTable = (strcmp(typeStr, "poker_table") == 0);

        if (!isItem && !isPokerTable) continue;

        Interactable* interactable = nullptr;
        if (isItem) {
            interactable = static_cast<Interactable*>(obj);
        } else if (isPokerTable) {
            interactable = static_cast<Interactable*>(obj);
        }

        if (!interactable->isActive) continue;

        Vector3 objPos = interactable->position;
        Vector3 toObj = Vector3Subtract(objPos, rayOrigin);

        float projection = Vector3DotProduct(toObj, rayDirection);

        if (projection < 0.1f || projection > maxInteractDistance) continue;

        Vector3 closestPointOnRay = Vector3Add(rayOrigin, Vector3Scale(rayDirection, projection));
        float distanceToRay = Vector3Distance(objPos, closestPointOnRay);

        float crosshairThreshold = 1.0f;

        if (distanceToRay < crosshairThreshold && projection < closestDistance) {
            closestDistance = projection;
            closestInteractable = interactable;
        }
    }

    return closestInteractable;
}

void Player::HandleShooting() {
    // Only shoot if left mouse button is pressed and we have an item selected
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

    TraceLog(LOG_INFO, "=== MOUSE CLICKED - HandleShooting called ===");

    if (selectedItemIndex < 0 || selectedItemIndex >= inventory.GetStackCount()) return;

    // Get the selected item
    ItemStack* stack = inventory.GetStack(selectedItemIndex);
    if (!stack || !stack->item) return;

    // Check if it's a pistol
    const char* itemType = stack->item->GetType();
    if (strcmp(itemType, "pistol") != 0) return;

    Pistol* pistol = static_cast<Pistol*>(stack->item);

    // Check if we can shoot
    if (!pistol->CanShoot()) {
        TraceLog(LOG_INFO, "Out of ammo!");
        return;
    }

    TraceLog(LOG_INFO, "Before shoot - Ammo: %d", pistol->GetAmmo());

    // Shoot the pistol (decrements ammo)
    pistol->Shoot();

    TraceLog(LOG_INFO, "After shoot - Ammo: %d", pistol->GetAmmo());

    // Create bullet from camera center
    Camera3D* cam = GetCamera();
    Vector3 bulletStart = cam->position;
    Vector3 direction = Vector3Normalize({
        cam->target.x - cam->position.x,
        cam->target.y - cam->position.y,
        cam->target.z - cam->position.z
    });

    // Create bullet and add to DOM
    Bullet* bullet = new Bullet(bulletStart, direction, 300.0f);  // Very fast bullet speed
    DOM* dom = DOM::GetGlobal();
    if (dom) {
        dom->AddObject(bullet);
    }

    TraceLog(LOG_INFO, "Bullet created! Total ammo now: %d", pistol->GetAmmo());

    // Check if pistol is out of ammo after shooting
    if (pistol->GetAmmo() <= 0) {
        TraceLog(LOG_INFO, "Pistol out of ammo! Removing from inventory and deleting.");

        // Remove from inventory
        inventory.RemoveItem(selectedItemIndex);

        // Delete the pistol immediately (inventory no longer references it)
        delete pistol;

        // Deselect item since it's gone
        selectedItemIndex = -1;
        lastHeldItemIndex = -1;
    }
}

void Player::DrawInventoryUI() {
    InventoryUI_Draw(&inventory, selectedItemIndex);
}

void Player::DrawHeldItem() {
    // Only draw if we have an item selected
    if (selectedItemIndex < 0 || selectedItemIndex >= inventory.GetStackCount()) {
        return;
    }

    ItemStack* stack = inventory.GetStack(selectedItemIndex);
    if (!stack || !stack->item) {
        return;
    }

    // Check if it's a pistol
    const char* itemType = stack->item->GetType();
    if (strcmp(itemType, "pistol") == 0) {
        Pistol* pistol = static_cast<Pistol*>(stack->item);
        Camera3D* cam = GetCamera();
        TraceLog(LOG_INFO, "Drawing held pistol at selected index %d", selectedItemIndex);
        pistol->DrawHeld(*cam);
    }
}
