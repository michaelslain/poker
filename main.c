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
#include <stdlib.h>
#include <stddef.h>

// DOM - Document Object Model (list of all objects in the scene)
typedef struct {
    Object** objects;
    int count;
    int capacity;
} DOM;

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

    // Use spawners to create objects
    int maxCards = 20;
    int maxChips = 25;
    Card* cards = (Card*)malloc(sizeof(Card) * maxCards);
    Chip* chips = (Chip*)malloc(sizeof(Chip) * maxChips);

    int cardCount = 0;
    int chipCount = 0;

    // Create spawners just above the plane
    Spawner cardSpawner;
    Spawner_Init(&cardSpawner, (Vector3){0.0f, 2.0f, 0.0f}, 2.0f);  // Radius 2.0

    Spawner chipSpawner;
    Spawner_Init(&chipSpawner, (Vector3){-5.0f, 2.0f, -5.0f}, 1.5f);  // Radius 1.5

    // Spawn some cards
    Spawner_SpawnCards(&cardSpawner, SUIT_SPADES, RANK_ACE, 3, &physics, cards, &cardCount);
    Spawner_SpawnCards(&cardSpawner, SUIT_HEARTS, RANK_KING, 2, &physics, cards, &cardCount);

    // Spawn some chips
    Spawner_SpawnChips(&chipSpawner, 1, 5, &physics, chips, &chipCount);
    Spawner_SpawnChips(&chipSpawner, 5, 5, &physics, chips, &chipCount);
    Spawner_SpawnChips(&chipSpawner, 10, 5, &physics, chips, &chipCount);
    Spawner_SpawnChips(&chipSpawner, 25, 5, &physics, chips, &chipCount);
    Spawner_SpawnChips(&chipSpawner, 100, 5, &physics, chips, &chipCount);

    // Add all spawned objects to DOM
    for (int i = 0; i < cardCount; i++) {
        DOM_AddObject(&dom, (Object*)&cards[i]);
    }

    for (int i = 0; i < chipCount; i++) {
        DOM_AddObject(&dom, (Object*)&chips[i]);
    }

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

        // Update all cards (sync physics to object positions)
        for (int i = 0; i < cardCount; i++) {
            Card_Update(&cards[i]);
        }

        // Update all chips
        for (int i = 0; i < chipCount; i++) {
            Chip_Update(&chips[i]);
        }

        // Find closest interactable (cards or chips)
        int closestCardIndex = -1;
        int closestChipIndex = -1;
        float closestDistance = 999999.0f;
        Vector3 playerPos = Player_GetPosition(&player);

        // Check cards
        for (int i = 0; i < cardCount; i++) {
            if (!cards[i].base.base.isActive) continue;

            float dist = Vector3Distance(playerPos, cards[i].base.base.base.position);

            if (dist <= cards[i].base.base.interactRange && dist < closestDistance) {
                closestDistance = dist;
                closestCardIndex = i;
                closestChipIndex = -1;
            }
        }

        // Check chips
        for (int i = 0; i < chipCount; i++) {
            if (!chips[i].base.base.isActive) continue;

            float dist = Vector3Distance(playerPos, chips[i].base.base.base.position);

            if (dist <= chips[i].base.base.interactRange && dist < closestDistance) {
                closestDistance = dist;
                closestChipIndex = i;
                closestCardIndex = -1;
            }
        }

        // Interact on E press
        if (IsKeyPressed(KEY_E)) {
            Inventory* inventory = Player_GetInventory(&player);

            if (closestCardIndex != -1) {
                // Pick up card
                Inventory_AddItem(inventory, &cards[closestCardIndex].base);
                DOM_RemoveObject(&dom, (Object*)&cards[closestCardIndex]);
                cards[closestCardIndex].base.base.isActive = false;

                TraceLog(LOG_INFO, "Card picked up! Inventory stacks: %d", inventory->stackCount);
            } else if (closestChipIndex != -1) {
                // Pick up chip
                Inventory_AddItem(inventory, &chips[closestChipIndex].base);
                DOM_RemoveObject(&dom, (Object*)&chips[closestChipIndex]);
                chips[closestChipIndex].base.base.isActive = false;

                TraceLog(LOG_INFO, "Chip picked up! Value: $%d, Inventory stacks: %d",
                        chips[closestChipIndex].value, inventory->stackCount);
            }
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

                // Draw all cards
                for (int i = 0; i < cardCount; i++) {
                    Card_Draw(&cards[i], camera);
                }

                // Draw all chips
                for (int i = 0; i < chipCount; i++) {
                    Chip_Draw(&chips[i], camera);
                }
            EndMode3D();

            // Apply darkening overlay when something is highlighted
            if (closestCardIndex != -1 || closestChipIndex != -1) {
                // Draw semi-transparent dark overlay over everything
                DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 100});

                BeginMode3D(camera);
                    if (closestCardIndex != -1) {
                        // Render highlighted card
                        rlPushMatrix();
                            Vector3 pos = cards[closestCardIndex].base.base.base.position;
                            Matrix rotMatrix = RigidBody_GetRotationMatrix(&cards[closestCardIndex].rigidBody);
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
                            rlSetTexture(cards[closestCardIndex].texture.texture.id);
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

                        Interactable_DrawPrompt(&cards[closestCardIndex].base.base, camera);
                    } else if (closestChipIndex != -1) {
                        // Render highlighted chip
                        Chip_Draw(&chips[closestChipIndex], camera);
                        Interactable_DrawPrompt(&chips[closestChipIndex].base.base, camera);
                    }
                EndMode3D();
            }

            // Draw inventory UI
            Player_DrawInventoryUI(&player);

            // Draw UI
            if (closestCardIndex != -1 || closestChipIndex != -1) {
                DrawText("Press E to interact", screenWidth / 2 - 80, screenHeight - 40, 20, GREEN);
            }

            DrawFPS(10, screenHeight - 30);

        EndDrawing();
    }

    // De-Initialization
    for (int i = 0; i < cardCount; i++) {
        Card_Cleanup(&cards[i]);
    }
    free(cards);

    for (int i = 0; i < chipCount; i++) {
        Chip_Cleanup(&chips[i]);
    }
    free(chips);

    Player_Cleanup(&player);
    Plane_Cleanup(&groundPlane);
    DOM_Cleanup(&dom);
    Physics_Cleanup(&physics);
    CloseWindow();

    return 0;
}
