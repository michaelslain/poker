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
#include "poker_table.h"
#include "dom.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>


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

    // Set global DOM for interaction detection
    DOM_SetGlobal(&dom);

    // Initialize player (camera is initialized inside player)
    Player player;
    Player_Init(&player, (Vector3){0.0f, 0.0f, 0.0f});
    DOM_AddObject(&dom, (Object*)&player);


    // Initialize ground plane with physics
    Plane groundPlane;
    Plane_Init(&groundPlane, (Vector3){0.0f, 0.0f, 0.0f}, (Vector2){50.0f, 50.0f}, LIGHTGRAY, &physics);
    DOM_AddObject(&dom, (Object*)&groundPlane);

    // Create poker table
    PokerTable* pokerTable = (PokerTable*)malloc(sizeof(PokerTable));
    Vector3 tableSize = {4.0f, 0.2f, 2.5f};  // Wide, thin, deep
    PokerTable_Init(pokerTable, (Vector3){5.0f, 1.0f, 0.0f}, tableSize, BROWN);
    DOM_AddObject(&dom, (Object*)pokerTable);

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
            
            // Get closest interactable for E prompt
            Interactable* closestInteractable = Player_GetClosestInteractable(&player);
            
            // Draw E prompt above closest interactable
            if (closestInteractable != NULL) {
                BeginMode3D(camera);
                    Interactable_DrawPrompt(closestInteractable, camera);
                EndMode3D();
            }

            Player_DrawInventoryUI(&player);

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
