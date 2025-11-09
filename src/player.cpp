#include "player.hpp"
#include "item.hpp"
#include "interactable.hpp"
#include "poker_table.hpp"
#include "wall.hpp"
#include "pistol.hpp"
#include "person.hpp"
#include "dom.hpp"
#include "inventory_ui.hpp"
#include "debug.hpp"
#include "card.hpp"
#include "raymath.h"
#include <cmath>
#include <cstring>
#include <vector>
#include <ode/ode.h>

Player::Player(Vector3 pos, PhysicsWorld* physicsWorld, const std::string& playerName)
    : Person(pos, playerName, 1.0f), camera({pos.x, pos.y + 1.7f, pos.z}), speed(5.0f),
      lookYaw(0.0f), lookPitch(0.0f), body(nullptr), geom(nullptr), physics(physicsWorld),
      selectedItemIndex(-1), lastHeldItemIndex(-1),
      bettingUIActive(false), bettingChoice(-1), raiseSliderValue(0), raiseMin(0), raiseMax(0),
      storedCurrentBet(0), storedCallAmount(0),
      cardSelectionUIActive(false), selectedCardIndices()
{
    if (physics != nullptr) {
        // Create dynamic body with mass for gravity
        body = dBodyCreate(physics->world);
        dBodySetPosition(body, pos.x, pos.y + 0.85f, pos.z);
        
        // Set mass for the player (needed for gravity)
        dMass mass;
        float radius = 0.4f;
        float height = 1.7f;
        float cylinderLength = height - (2.0f * radius);
        dMassSetCapsuleTotal(&mass, 70.0f, 3, radius, cylinderLength); // 70kg player
        dBodySetMass(body, &mass);
        
        // Disable auto-disable so player doesn't "sleep" and fall through floor
        dBodySetAutoDisableFlag(body, 0);

        // Create capsule geometry
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

std::string Player::GetType() const {
    return "player";
}

void Player::HandleInteraction() {
    if (!IsKeyPressed(KEY_E)) return;

    Interactable* closestInteractable = GetClosestInteractable();
    if (!closestInteractable) return;

    std::string typeStr = closestInteractable->GetType();

    // Handle poker table interaction (sit down or stand up)
    if (typeStr == "poker_table") {
        PokerTable* table = static_cast<PokerTable*>(closestInteractable);
        
        // Check if we're already seated at this table
        int seatIndex = table->FindSeatIndex(this);
        if (seatIndex != -1) {
            // Already seated, stand up
            table->UnseatPerson(this);
        } else {
            // Not seated, find closest seat and sit down
            int closestSeat = table->FindClosestOpenSeat(position);
            if (closestSeat != -1) {
                table->SeatPerson(this, closestSeat);
            } else {
                printf("Table is full!\n");
            }
        }
    }
    // Handle item pickup
    else if (typeStr.substr(0, 5) == "card_" || typeStr.substr(0, 5) == "chip_" || typeStr == "pistol") {
        Item* item = static_cast<Item*>(closestInteractable);

        // Add to inventory
        inventory.AddItem(item);

        // Remove from DOM - inventory now owns it
        DOM* dom = DOM::GetGlobal();
        if (dom) {
            dom->RemoveObject(item);
        }

        TraceLog(LOG_INFO, "Item picked up: %s, Inventory stacks: %d", typeStr.c_str(), inventory.GetStackCount());
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

    // Body rotation follows camera when turning more than 45 degrees
    float maxHeadTurnAngle = 45.0f * DEG2RAD;  // 45 degrees in radians
    float angleDifference = lookYaw - bodyYaw;
    
    // Normalize angle difference to [-PI, PI]
    while (angleDifference > PI) angleDifference -= 2 * PI;
    while (angleDifference < -PI) angleDifference += 2 * PI;
    
    // If head is turned more than 45 degrees, rotate body to follow
    if (angleDifference > maxHeadTurnAngle) {
        bodyYaw = lookYaw - maxHeadTurnAngle;
    } else if (angleDifference < -maxHeadTurnAngle) {
        bodyYaw = lookYaw + maxHeadTurnAngle;
    }

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

    // WASD movement (disabled when seated)
    Vector3 moveDir = {0.0f, 0.0f, 0.0f};

    if (isSeated) {
        // When seated, lock XZ position to seat but keep Y at ground level
        // This keeps camera height consistent
        position.x = seatPosition.x;
        position.z = seatPosition.z;
        position.y = 0.0f;  // Keep at ground level for consistent camera height
        
        // Lock physics body to ground position at seat XZ
        if (body != nullptr) {
            dBodySetPosition(body, seatPosition.x, 0.85f, seatPosition.z);
            // Reset velocity when seated
            dBodySetLinearVel(body, 0, 0, 0);
            dBodySetAngularVel(body, 0, 0, 0);
        }
        if (geom != nullptr) {
            dGeomSetPosition(geom, seatPosition.x, 0.85f, seatPosition.z);
        }
        
        // Skip movement processing when seated
    } else {
        // Only process movement when not seated
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
    }

    // Normalize and apply speed (only when not seated)
    if (!isSeated && Vector3Length(moveDir) > 0) {
        moveDir = Vector3Normalize(moveDir);
        moveDir = Vector3Scale(moveDir, speed * deltaTime);

        Vector3 oldPos = position;
        Vector3 newPos = Vector3Add(position, moveDir);

        // Update physics body position and check for collisions
        if (body != nullptr && geom != nullptr) {
            // Helper lambda to extract geometry from any object type
            auto getGeomFromObject = [](Object* obj) -> dGeomID {
                std::string typeStr = obj->GetType();
                if (typeStr == "poker_table") {
                    PokerTable* table = static_cast<PokerTable*>(obj);
                    return table->GetGeom();
                } else if (typeStr == "wall") {
                    Wall* wall = static_cast<Wall*>(obj);
                    return wall->GetGeom();
                }
                // Add more collider types here as needed
                return nullptr;
            };

            Vector3 finalPos = newPos;
            const int maxIterations = 3;  // Handle corners and complex geometry

            for (int iteration = 0; iteration < maxIterations; iteration++) {
                // Test the position
                dGeomSetPosition(geom, finalPos.x, finalPos.y + 0.85f, finalPos.z);

                // Check for collisions
                bool collided = false;
                Vector3 collisionNormal = {0.0f, 0.0f, 0.0f};
                DOM* dom = DOM::GetGlobal();

                if (dom) {
                    for (int i = 0; i < dom->GetCount(); i++) {
                        Object* obj = dom->GetObject(i);
                        dGeomID otherGeom = getGeomFromObject(obj);

                        if (otherGeom != nullptr) {
                            dContactGeom contacts[4];
                            int numContacts = dCollide(geom, otherGeom, 4, contacts, sizeof(dContactGeom));
                            if (numContacts > 0) {
                                collided = true;
                                // Use the first contact normal
                                collisionNormal.x = contacts[0].normal[0];
                                collisionNormal.y = contacts[0].normal[1];
                                collisionNormal.z = contacts[0].normal[2];
                                break;
                            }
                        }
                    }
                }

                if (!collided) {
                    // No collision, accept this position
                    break;
                }

                // Collision detected - slide along the surface
                // Calculate movement vector relative to where we currently are
                Vector3 movementVec = Vector3Subtract(finalPos, oldPos);
                
                // Only slide if moving into the surface
                float dotProduct = Vector3DotProduct(movementVec, collisionNormal);
                if (dotProduct < 0) {
                    // Project movement onto the plane perpendicular to collision normal
                    Vector3 projectedMovement = Vector3Subtract(movementVec, Vector3Scale(collisionNormal, dotProduct));
                    finalPos = Vector3Add(oldPos, projectedMovement);
                } else {
                    // Moving away from surface - allow it
                    break;
                }
            }

            // Get current Y position from physics (affected by gravity)
            const dReal* physicsPos = dBodyGetPosition(body);
            float currentY = (float)physicsPos[1] - 0.85f;
            
            // Set final XZ position, preserve Y from physics
            finalPos.y = currentY;
            dGeomSetPosition(geom, finalPos.x, finalPos.y + 0.85f, finalPos.z);
            dBodySetPosition(body, finalPos.x, finalPos.y + 0.85f, finalPos.z);
            position = finalPos;
            
            // Reset horizontal velocity to prevent sliding, preserve vertical velocity for gravity
            const dReal* vel = dBodyGetLinearVel(body);
            dBodySetLinearVel(body, 0, vel[1], 0);
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
        std::string typeStr = obj->GetType();

        bool isItem = (typeStr.substr(0, 5) == "card_" || typeStr.substr(0, 5) == "chip_" || typeStr == "pistol");
        bool isPokerTable = (typeStr == "poker_table");

        if (!isItem && !isPokerTable) continue;

        Interactable* interactable = nullptr;
        if (isItem) {
            interactable = static_cast<Interactable*>(obj);
        } else if (isPokerTable) {
            interactable = static_cast<Interactable*>(obj);
        }



        Vector3 objPos = interactable->position;
        Vector3 toObj = Vector3Subtract(objPos, rayOrigin);

        float projection = Vector3DotProduct(toObj, rayDirection);

        if (projection < 0.1f || projection > maxInteractDistance) continue;

        Vector3 closestPointOnRay = Vector3Add(rayOrigin, Vector3Scale(rayDirection, projection));
        float distanceToRay = Vector3Distance(objPos, closestPointOnRay);

        float crosshairThreshold = 1.0f;

        // Only consider interactables that have canInteract enabled
        if (distanceToRay < crosshairThreshold && projection < closestDistance && interactable->canInteract) {
            closestDistance = projection;
            closestInteractable = interactable;
        }
    }

    return closestInteractable;
}

void Player::HandleShooting() {
    // Only shoot if left mouse button is pressed and we have an item selected
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }

    GAME_LOG(LOG_INFO, "=== MOUSE CLICKED - HandleShooting called ===");

    if (selectedItemIndex < 0 || selectedItemIndex >= inventory.GetStackCount()) {
        GAME_LOG(LOG_INFO, "Invalid selectedItemIndex, returning");
        return;
    }

    // Get the selected item
    ItemStack* stack = inventory.GetStack(selectedItemIndex);
    if (!stack || !stack->item) return;

    // Check if it's a pistol
    std::string itemType = stack->item->GetType();
    if (itemType != "pistol") return;

    Pistol* pistol = static_cast<Pistol*>(stack->item);

    // Check if we can shoot
    if (!pistol->CanShoot()) {
        GAME_LOG(LOG_INFO, "Out of ammo!");
        return;
    }

    GAME_LOG(LOG_INFO, "Before shoot - Ammo: %d", pistol->GetAmmo());

    // Shoot the pistol (decrements ammo)
    pistol->Shoot();

    GAME_LOG(LOG_INFO, "After shoot - Ammo: %d", pistol->GetAmmo());


    // Instant raycast from camera
    Camera3D* cam = GetCamera();
    Vector3 rayStart = cam->position;
    Vector3 direction = Vector3Normalize({
        cam->target.x - cam->position.x,
        cam->target.y - cam->position.y,
        cam->target.z - cam->position.z
    });
    
    // Cast ray through all people in the DOM
    DOM* dom = DOM::GetGlobal();
    if (dom) {
        float maxDistance = 1000.0f;  // Max shooting range
        Person* hitPerson = nullptr;
        float closestHit = maxDistance;
        
        for (int i = 0; i < dom->GetCount(); i++) {
            Object* obj = dom->GetObject(i);
            if (!obj) continue;
            
            std::string type = obj->GetType();
            if (type == "player" || 
                type == "enemy" || 
                type == "dealer") {
                
                Person* person = static_cast<Person*>(obj);
                
                // Don't shoot yourself!
                if (person == this) continue;
                
                // Cylinder collision: check if ray intersects the person's hitbox
                float personHeight = person->GetHeight();
                float personTopY = person->position.y + 2.4f * personHeight;
                float personBottomY = person->position.y;
                float hitRadius = 0.5f;
                
                // For each point along the ray, check cylinder intersection
                for (float dist = 0; dist < maxDistance; dist += 0.1f) {
                    Vector3 rayPoint = {
                        rayStart.x + direction.x * dist,
                        rayStart.y + direction.y * dist,
                        rayStart.z + direction.z * dist
                    };
                    
                    // Check if within height range
                    if (rayPoint.y >= personBottomY && rayPoint.y <= personTopY) {
                        // Check horizontal distance
                        float dx = rayPoint.x - person->position.x;
                        float dz = rayPoint.z - person->position.z;
                        float horizontalDist = sqrtf(dx*dx + dz*dz);
                        
                        if (horizontalDist < hitRadius && dist < closestHit) {
                            hitPerson = person;
                            closestHit = dist;
                            break;  // Hit this person, stop checking this person
                        }
                    }
                }
            }
        }
        
        // If we hit someone, kill them
        if (hitPerson) {
            GAME_LOG(LOG_INFO, "*** SHOT HIT %s at distance %.2f! ***", hitPerson->GetName().c_str(), closestHit);
            
            // If person is sitting, unseat them from all poker tables
            if (hitPerson->IsSeated()) {
                for (int j = 0; j < dom->GetCount(); j++) {
                    Object* tableObj = dom->GetObject(j);
                    if (tableObj) {
                        std::string tableType = tableObj->GetType();
                        if (tableType == "poker_table") {
                            PokerTable* table = static_cast<PokerTable*>(tableObj);
                            table->UnseatPerson(hitPerson);
                        }
                    }
                }
            }
            
            // Remove person from DOM and delete
            DOM::GetGlobal()->RemoveAndDelete(hitPerson);
        } else {
            GAME_LOG(LOG_INFO, "Shot missed!");
        }
    }
    // Check if pistol is out of ammo after shooting
    if (pistol->GetAmmo() <= 0) {
        GAME_LOG(LOG_INFO, "Pistol out of ammo! Removing from inventory and deleting.");

        // Remove from inventory
        inventory.RemoveItem(selectedItemIndex);

        // Delete the pistol immediately (inventory no longer references it)
        delete pistol;

        // Deselect item since it's gone
        selectedItemIndex = -1;
        lastHeldItemIndex = -1;
    }
}

void Player::Draw(Camera3D camera) {
    // Call parent's draw (draws the person model)
    Person::Draw(camera);
    
    // Draw player's capsule collision geometry if debug mode is on
    if (g_showCollisionDebug && geom) {
        dReal radius, length;
        dGeomCapsuleGetParams(geom, &radius, &length);
        
        // Draw wireframe capsule at player position
        Vector3 topPos = {position.x, position.y + (float)length, position.z};
        DrawCapsuleWires(position, topPos, (float)radius, 8, 8, LIME);
    }
}

void Player::DrawInventoryUI() {
    InventoryUI_Draw(&inventory, selectedItemIndex);
    
    // Draw card selection UI if active
    DrawCardSelectionUI();
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
    std::string itemType = stack->item->GetType();
    if (itemType.empty()) {
        return;  // Safety check - GetType() returned null
    }
    
    if (itemType == "pistol") {
        Pistol* pistol = static_cast<Pistol*>(stack->item);
        if (!pistol) {
            return;  // Safety check - cast failed
        }
        Camera3D* cam = GetCamera();
        pistol->DrawHeld(*cam);
    }
}

void Player::SitDown(Vector3 seatPos) {
    // Call base class to set seating state
    Person::SitDown(seatPos);
    
    // Update physics body position and reset velocity
    if (body != nullptr) {
        dBodySetPosition(body, seatPos.x, seatPos.y + 0.85f, seatPos.z);
        // Reset velocity when sitting
        dBodySetLinearVel(body, 0, 0, 0);
        dBodySetAngularVel(body, 0, 0, 0);
    }
    if (geom != nullptr) {
        dGeomSetPosition(geom, seatPos.x, seatPos.y + 0.85f, seatPos.z);
    }
}

void Player::StandUp() {
    // Call base class to clear seating state
    Person::StandUp();
    
    // Move player to ground level when standing (prevents clipping through table)
    // Seat positions are at table height, we need to move to ground
    position.y = 0.0f;  // Reset to ground level
    
    // Update physics body to ground position
    if (body != nullptr) {
        dBodySetPosition(body, position.x, position.y + 0.85f, position.z);
        // Reset velocity so player doesn't fly off
        dBodySetLinearVel(body, 0, 0, 0);
        dBodySetAngularVel(body, 0, 0, 0);
    }
    if (geom != nullptr) {
        dGeomSetPosition(geom, position.x, position.y + 0.85f, position.z);
    }
}

int Player::PromptBet(int currentBet, int callAmount, int minRaise, int maxRaise, int& raiseAmount) {
    GAME_LOG(LOG_INFO, "Player::PromptBet called - bettingUIActive=%d, bettingChoice=%d", bettingUIActive, bettingChoice);
    
    // First call: Initialize UI
    if (!bettingUIActive) {
        bettingUIActive = true;
        bettingChoice = -1;  // Reset choice
        raiseMin = minRaise;
        raiseMax = maxRaise;
        raiseSliderValue = minRaise;  // Default to minimum raise
        storedCurrentBet = currentBet;
        storedCallAmount = callAmount;
        GAME_LOG(LOG_INFO, "Initialized betting UI");
    }
    
    // Wait for player to make a choice (poker table will call this repeatedly each frame)
    // Returns -1 while waiting, returns choice when made
    if (bettingChoice == -1) {
        return -1;  // Still waiting for input
    }
    
    // Choice was made
    bettingUIActive = false;
    
    if (bettingChoice == 2) {  // Raise
        raiseAmount = raiseSliderValue;
    }
    
    int choice = bettingChoice;
    bettingChoice = -1;  // Reset for next time
    return choice;
}

void Player::DrawBettingUI() {
    if (!bettingUIActive) return;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw semi-transparent background
    DrawRectangle(0, screenHeight - 200, screenWidth, 200, (Color){0, 0, 0, 200});
    
    // Draw title
    DrawText("YOUR TURN TO BET", screenWidth / 2 - 100, screenHeight - 190, 20, WHITE);
    DrawText(TextFormat("Current Bet: %d | To Call: %d", storedCurrentBet, storedCallAmount), screenWidth / 2 - 120, screenHeight - 160, 16, YELLOW);
    
    // Draw options
    int buttonY = screenHeight - 120;
    int buttonWidth = 150;
    int buttonHeight = 40;
    int spacing = 20;
    
    // Fold button (1)
    Rectangle foldButton = {(float)(screenWidth / 2 - buttonWidth - spacing - buttonWidth / 2), (float)buttonY, (float)buttonWidth, (float)buttonHeight};
    Color foldColor = DARKGRAY;
    if (IsKeyPressed(KEY_ONE)) {
        bettingChoice = 0;
    }
    DrawRectangleRec(foldButton, foldColor);
    DrawText("FOLD", (int)foldButton.x + 45, (int)foldButton.y + 10, 20, WHITE);
    DrawText("1", (int)foldButton.x + 65, (int)foldButton.y + 32, 10, LIGHTGRAY);
    
    // Call/Check button (2)
    Rectangle callButton = {(float)(screenWidth / 2 - buttonWidth / 2), (float)buttonY, (float)buttonWidth, (float)buttonHeight};
    Color callColor = GREEN;
    if (IsKeyPressed(KEY_TWO)) {
        bettingChoice = 1;
    }
    DrawRectangleRec(callButton, callColor);
    
    // Show "CHECK" if call amount is 0, otherwise show "CALL X"
    if (storedCallAmount == 0) {
        DrawText("CHECK", (int)callButton.x + 40, (int)callButton.y + 10, 20, WHITE);
    } else {
        DrawText(TextFormat("CALL %d", storedCallAmount), (int)callButton.x + 30, (int)callButton.y + 10, 20, WHITE);
    }
    DrawText("2", (int)callButton.x + 65, (int)callButton.y + 32, 10, LIGHTGRAY);
    
    // Raise button (3)
    Rectangle raiseButton = {(float)(screenWidth / 2 + spacing + buttonWidth / 2), (float)buttonY, (float)buttonWidth, (float)buttonHeight};
    Color raiseColor = RED;
    DrawRectangleRec(raiseButton, raiseColor);
    DrawText("RAISE", (int)raiseButton.x + 40, (int)raiseButton.y + 10, 20, WHITE);
    DrawText("3", (int)raiseButton.x + 65, (int)raiseButton.y + 32, 10, LIGHTGRAY);
    
    // Always show slider for raise option
    {
        int sliderY = screenHeight - 60;
        int sliderX = screenWidth / 2 - 200;
        int sliderWidth = 400;
        
        // Draw slider background
        DrawRectangle(sliderX, sliderY, sliderWidth, 20, DARKGRAY);
        
        // Handle left/right arrow keys to adjust raise amount
        if (IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT)) {
            raiseSliderValue -= 5;
            if (raiseSliderValue < raiseMin) raiseSliderValue = raiseMin;
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT)) {
            raiseSliderValue += 5;
            if (raiseSliderValue > raiseMax) raiseSliderValue = raiseMax;
        }
        
        // Calculate slider position (avoid division by zero)
        float sliderPercent = 0.0f;
        if (raiseMax > raiseMin) {
            sliderPercent = (float)(raiseSliderValue - raiseMin) / (float)(raiseMax - raiseMin);
        }
        int sliderPos = sliderX + (int)(sliderPercent * sliderWidth);
        
        // Draw slider
        DrawRectangle(sliderX, sliderY, (int)(sliderPercent * sliderWidth), 20, RED);
        DrawCircle(sliderPos, sliderY + 10, 12, ORANGE);
        
        // Draw raise amount
        DrawText(TextFormat("Raise to: %d (Min: %d, Max: %d)", raiseSliderValue, raiseMin, raiseMax),
                 sliderX, sliderY - 25, 16, WHITE);
        DrawText("Use LEFT/RIGHT arrows to adjust, press 3 to confirm", sliderX + 40, sliderY + 25, 12, LIGHTGRAY);
    }
    
    // Confirm raise with KEY_THREE
    if (IsKeyPressed(KEY_THREE)) {
        bettingChoice = 2;
    }
}

void Player::DrawCardSelectionUI() {
    if (!cardSelectionUIActive) return;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Count cards in inventory
    std::vector<int> cardIndices;
    for (int i = 0; i < inventory.GetStackCount(); i++) {
        ItemStack* stack = inventory.GetStack(i);
        if (stack && stack->item && stack->item->GetType().substr(0, 5) == "card_") {
            cardIndices.push_back(i);
        }
    }
    
    // Draw semi-transparent background
    DrawRectangle(0, screenHeight / 2 - 150, screenWidth, 300, (Color){0, 0, 0, 220});
    
    // Draw title
    DrawText("SELECT 2 CARDS FOR YOUR HAND", screenWidth / 2 - 200, screenHeight / 2 - 140, 24, YELLOW);
    DrawText(TextFormat("Selected: %d/2", (int)selectedCardIndices.size()), screenWidth / 2 - 80, screenHeight / 2 - 110, 20, WHITE);
    DrawText("Click on cards to select/deselect. Press ENTER when done.", screenWidth / 2 - 240, screenHeight / 2 - 85, 16, LIGHTGRAY);
    
    // Draw cards as clickable boxes
    int cardWidth = 80;
    int cardHeight = 120;
    int spacing = 20;
    int startX = screenWidth / 2 - ((cardWidth + spacing) * cardIndices.size()) / 2;
    int startY = screenHeight / 2 - 40;
    
    for (size_t i = 0; i < cardIndices.size(); i++) {
        int invIndex = cardIndices[i];
        ItemStack* stack = inventory.GetStack(invIndex);
        if (!stack || !stack->item) continue;
        
        Card* card = static_cast<Card*>(stack->item);
        
        int x = startX + i * (cardWidth + spacing);
        int y = startY;
        
        Rectangle cardRect = {(float)x, (float)y, (float)cardWidth, (float)cardHeight};
        
        // Check if this card is selected
        bool isSelected = false;
        for (int selIdx : selectedCardIndices) {
            if (selIdx == invIndex) {
                isSelected = true;
                break;
            }
        }
        
        // Handle mouse click
        if (CheckCollisionPointRec(GetMousePosition(), cardRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (isSelected) {
                // Deselect
                for (size_t j = 0; j < selectedCardIndices.size(); j++) {
                    if (selectedCardIndices[j] == invIndex) {
                        selectedCardIndices.erase(selectedCardIndices.begin() + j);
                        break;
                    }
                }
            } else {
                // Select (if not already 2 selected)
                if (selectedCardIndices.size() < 2) {
                    selectedCardIndices.push_back(invIndex);
                }
            }
        }
        
        // Draw card
        Color borderColor = isSelected ? GOLD : WHITE;
        int borderWidth = isSelected ? 4 : 2;
        
        DrawRectangleRec(cardRect, WHITE);
        DrawRectangleLinesEx(cardRect, borderWidth, borderColor);
        
        // Draw card icon (simplified - just show rank and suit)
        const char* rankStr = Card::GetRankString(card->rank);
        const char* suitSymbol = Card::GetSuitSymbol(card->suit);
        Color suitColor = Card::GetSuitColor(card->suit);
        
        DrawText(rankStr, x + 10, y + 10, 20, BLACK);
        DrawText(suitSymbol, x + 10, y + 35, 30, suitColor);
    }
    
    // Draw confirm button
    Rectangle confirmButton = {(float)(screenWidth / 2 - 100), (float)(screenHeight / 2 + 100), 200.0f, 40.0f};
    Color confirmColor = selectedCardIndices.size() == 2 ? GREEN : DARKGRAY;
    DrawRectangleRec(confirmButton, confirmColor);
    DrawText("CONFIRM", (int)confirmButton.x + 50, (int)confirmButton.y + 10, 20, WHITE);
    DrawText("ENTER", (int)confirmButton.x + 70, (int)confirmButton.y + 32, 10, LIGHTGRAY);
    
    // Confirm with ENTER
    if (IsKeyPressed(KEY_ENTER) && selectedCardIndices.size() == 2) {
        cardSelectionUIActive = false;
    }
}

std::vector<Card*> Player::GetSelectedCards() {
    std::vector<Card*> cards;
    
    // If no selection active or not enough cards selected, return all cards
    if (!cardSelectionUIActive && selectedCardIndices.empty()) {
        for (int i = 0; i < inventory.GetStackCount(); i++) {
            ItemStack* stack = inventory.GetStack(i);
            if (stack && stack->item && stack->item->GetType().substr(0, 5) == "card_") {
                cards.push_back(static_cast<Card*>(stack->item));
            }
        }
        return cards;
    }
    
    // Return only selected cards
    for (int idx : selectedCardIndices) {
        ItemStack* stack = inventory.GetStack(idx);
        if (stack && stack->item && stack->item->GetType().substr(0, 5) == "card_") {
            cards.push_back(static_cast<Card*>(stack->item));
        }
    }
    
    return cards;
}
