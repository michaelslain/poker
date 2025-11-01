#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "player.h"
#include "camera.h"
#include "interactable.h"
#include "card.h"
#include "chip.h"
#include "plane.h"
#include "physics.h"
#include "spawner.h"
#include "dom.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

void DOM_Init(DOM* dom, int initialCapacity) {
    dom->objects = (Object**)malloc(sizeof(Object*) * initialCapacity);
    dom->count = 0;
    dom->capacity = initialCapacity;
}

void DOM_AddObject(DOM* dom, Object* obj) {
    if (dom->count >= dom->capacity) {
        dom->capacity *= 2;
        dom->objects = (Object**)realloc(dom->objects, sizeof(Object*) * dom->capacity);
    }
    dom->objects[dom->count++] = obj;
}

void DOM_RemoveObject(DOM* dom, Object* obj) {
    for (int i = 0; i < dom->count; i++) {
        if (dom->objects[i] == obj) {
            // Shift all objects after this one down
            for (int j = i; j < dom->count - 1; j++) {
                dom->objects[j] = dom->objects[j + 1];
            }
            dom->count--;
            return;
        }
    }
}

void DOM_Cleanup(DOM* dom) {
    free(dom->objects);
    dom->objects = NULL;
    dom->count = 0;
    dom->capacity = 0;
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Poker - First Person");

    // Disable cursor for FPS controls
    DisableCursor();

    // Initialize physics world
    PhysicsWorld physics;
    Physics_Init(&physics);

    // Initialize DOM
    DOM dom;
    DOM_Init(&dom, 50);

    // Initialize player (camera is initialized inside player)
    Player player;
    Player_Init(&player, (Vector3){0.0f, 0.0f, 0.0f});
    DOM_AddObject(&dom, (Object*)&player);

    // Initialize ground plane with physics
    Plane groundPlane;
    Plane_Init(&groundPlane, (Vector3){0.0f, 0.0f, 0.0f}, (Vector2){50.0f, 50.0f}, LIGHTGRAY, &physics);
    DOM_AddObject(&dom, (Object*)&groundPlane);

    // Create spawners just above the plane
    Spawner cardSpawner;
    Spawner_Init(&cardSpawner, (Vector3){0.0f, 2.0f, 0.0f}, 2.0f);  // Radius 2.0

    Spawner chipSpawner;
    Spawner_Init(&chipSpawner, (Vector3){-5.0f, 2.0f, -5.0f}, 1.5f);  // Radius 1.5

    // Spawn some cards (spawner allocates and adds to DOM)
    Spawner_SpawnCards(&cardSpawner, SUIT_SPADES, RANK_ACE, 3, &physics, &dom);
    Spawner_SpawnCards(&cardSpawner, SUIT_HEARTS, RANK_KING, 2, &physics, &dom);

    // Spawn some chips (spawner allocates and adds to DOM)
    Spawner_SpawnChips(&chipSpawner, 1, 5, &physics, &dom);
    Spawner_SpawnChips(&chipSpawner, 5, 5, &physics, &dom);
    Spawner_SpawnChips(&chipSpawner, 10, 5, &physics, &dom);
    Spawner_SpawnChips(&chipSpawner, 25, 5, &physics, &dom);
    Spawner_SpawnChips(&chipSpawner, 100, 5, &physics, &dom);

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Toggle cursor lock with U key
        if (IsKeyPressed(KEY_U)) {
            if (IsCursorHidden()) {
                EnableCursor();
            } else {
                DisableCursor();
            }
        }

        // Update player (handles input, camera, and FOV)
        Player_Update(&player, deltaTime);

        // Step physics simulation
        Physics_Step(&physics, deltaTime);

        // Update all objects in DOM
        for (int i = 0; i < dom.count; i++) {
            Object* obj = dom.objects[i];
            if (obj != NULL && obj->update != NULL) {
                obj->update(obj, deltaTime);
            }
        }

        // Find closest interactable item in DOM
        Item* closestItem = NULL;
        float closestDistance = 999999.0f;
        Vector3 playerPos = Player_GetPosition(&player);

        for (int i = 0; i < dom.count; i++) {
            Object* obj = dom.objects[i];
            
            // Check if it's an item (has getType and type starts with "card_" or "chip_")
            if (obj->getType == NULL) continue;
            const char* typeStr = obj->getType(obj);
            if (strncmp(typeStr, "card_", 5) != 0 && strncmp(typeStr, "chip_", 5) != 0) continue;
            
            // It's an item - cast to Item
            Item* item = (Item*)obj;
            Interactable* interactable = &item->base;
            if (!interactable->isActive) continue;

            float dist = Vector3Distance(playerPos, interactable->base.position);

            if (dist <= interactable->interactRange && dist < closestDistance) {
                closestDistance = dist;
                closestItem = item;
            }
        }

        // Interact on E press
        if (IsKeyPressed(KEY_E) && closestItem != NULL) {
            Inventory* inventory = Player_GetInventory(&player);
            
            // Add to inventory and deactivate (keep in DOM for reference)
            Inventory_AddItem(inventory, closestItem);
            closestItem->base.isActive = false;
            
            const char* typeStr = closestItem->base.base.getType((Object*)closestItem);
            TraceLog(LOG_INFO, "Item picked up: %s, Inventory stacks: %d", typeStr, inventory->stackCount);
        }

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            Camera3D camera = *Player_GetCamera(&player);

            // Render main scene
            BeginMode3D(camera);
                // Draw ground plane
                Plane_Draw(&groundPlane);
                DrawGrid(50, 1.0f);

                // Draw all objects in DOM
                for (int i = 0; i < dom.count; i++) {
                    Object* obj = dom.objects[i];
                    if (obj != NULL && obj->draw != NULL) {
                        obj->draw(obj, camera);
                    }
                }
            EndMode3D();

            // Apply darkening overlay when something is highlighted
            if (closestItem != NULL) {
                // Draw semi-transparent dark overlay over everything
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 100});

                BeginMode3D(camera);
                    const char* typeStr = closestItem->base.base.getType((Object*)closestItem);
                    
                    if (strncmp(typeStr, "card_", 5) == 0) {
                        Card* card = (Card*)closestItem;
                        // Render highlighted card
                        rlPushMatrix();
                            Vector3 pos = card->base.base.base.position;
                            Matrix rotMatrix = RigidBody_GetRotationMatrix(&card->rigidBody);
                            Matrix transMatrix = MatrixTranslate(pos.x, pos.y, pos.z);
                            Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
                            rlMultMatrixf(MatrixToFloat(transform));

                            float cardWidth = 0.5f;
                            float cardHeight = 0.7f;
                            float cardThickness = 0.02f;

                            DrawCube((Vector3){0, 0, 0}, cardWidth, cardHeight, cardThickness, WHITE);
                            DrawCubeWires((Vector3){0, 0, 0}, cardWidth, cardHeight, cardThickness,
                                         (Color){100, 100, 100, 255});

                            rlTranslatef(0, 0, cardThickness/2 + 0.01f);
                            rlSetTexture(card->texture.texture.id);
                            rlBegin(RL_QUADS);
                                rlColor4ub(255, 255, 255, 255);
                                rlNormal3f(0.0f, 0.0f, 1.0f);
                                rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-cardWidth/2, -cardHeight/2, 0.0f);
                                rlTexCoord2f(1.0f, 0.0f); rlVertex3f(cardWidth/2, -cardHeight/2, 0.0f);
                                rlTexCoord2f(1.0f, 1.0f); rlVertex3f(cardWidth/2, cardHeight/2, 0.0f);
                                rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-cardWidth/2, cardHeight/2, 0.0f);
                            rlEnd();
                            rlSetTexture(0);
                        rlPopMatrix();

                        Interactable_DrawPrompt(&card->base.base, camera);
                    } else if (strncmp(typeStr, "chip_", 5) == 0) {
                        Chip* chip = (Chip*)closestItem;
                        // Render highlighted chip
                        Chip_Draw(chip, camera);
                        Interactable_DrawPrompt(&chip->base.base, camera);
                    }
                EndMode3D();
            }

            // Draw inventory UI
            Player_DrawInventoryUI(&player);

            // Draw UI
            if (closestItem != NULL) {
                DrawText("Press E to interact", screenWidth / 2 - 80, screenHeight - 40, 20, GREEN);
            }

            DrawFPS(10, screenHeight - 30);

        EndDrawing();
    }

    // De-Initialization
    // Clean up all dynamically allocated objects in DOM
    for (int i = 0; i < dom.count; i++) {
        Object* obj = dom.objects[i];
        
        // Only free if it was dynamically allocated
        if (obj->isDynamicallyAllocated) {
            // Check if it's an Item and clean it up
            if (obj->getType != NULL) {
                const char* typeStr = obj->getType(obj);
                
                if (strncmp(typeStr, "card_", 5) == 0) {
                    Card_Cleanup((Card*)obj);
                } else if (strncmp(typeStr, "chip_", 5) == 0) {
                    Chip_Cleanup((Chip*)obj);
                }
            }
            free(obj);
        }
    }

    Player_Cleanup(&player);
    Plane_Cleanup(&groundPlane);
    DOM_Cleanup(&dom);
    Physics_Cleanup(&physics);
    CloseWindow();

    return 0;
}
