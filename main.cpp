#include "raylib.h"
#include "rlgl.h"
#include "interactable.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "pistol.hpp"
#include "bullet.hpp"
#include "floor.hpp"
#include "ceiling.hpp"
#include "wall.hpp"
#include "light.hpp"
#include "light_bulb.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "physics.hpp"
#include "spawner.hpp"
#include "poker_table.hpp"
#include "dom.hpp"
#include <cstdlib>
#include <cstring>

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

    // Initialize lighting system (loads shader, sets up ambient light)
    LightSource::InitLightingSystem();

    // Initialize DOM
    DOM dom;
    DOM::SetGlobal(&dom);

    // Initialize player (using C++ new and constructor)
    Player* player = new Player({0.0f, 0.0f, 0.0f}, &physics, "Player");
    dom.AddObject(player);

    // Initialize ground floor with physics
    Floor* groundFloor = new Floor({0.0f, 0.0f, 0.0f}, {50.0f, 50.0f}, {50, 0, 12, 255}, &physics);
    dom.AddObject(groundFloor);

    // Create ceiling (black)
    Ceiling* ceiling = new Ceiling({0.0f, 5.0f, 0.0f}, {50.0f, 50.0f}, BLACK, &physics);
    dom.AddObject(ceiling);

    // Create 4 walls to make a room (20x20 room centered at origin)
    Wall* wallNorth = new Wall({0.0f, 2.5f, 10.0f}, {20.0f, 5.0f, 0.5f}, &physics);   // North wall
    Wall* wallSouth = new Wall({0.0f, 2.5f, -10.0f}, {20.0f, 5.0f, 0.5f}, &physics);  // South wall
    Wall* wallEast = new Wall({10.0f, 2.5f, 0.0f}, {0.5f, 5.0f, 20.0f}, &physics);    // East wall
    Wall* wallWest = new Wall({-10.0f, 2.5f, 0.0f}, {0.5f, 5.0f, 20.0f}, &physics);   // West wall
    dom.AddObject(wallNorth);
    dom.AddObject(wallSouth);
    dom.AddObject(wallEast);
    dom.AddObject(wallWest);

    // Create light bulb at center of room (blue light)
    LightBulb* lightBulb = new LightBulb({0.0f, 4.0f, 0.0f}, (Color){120, 140, 200, 255});
    dom.AddObject(lightBulb);

    // Create poker table inside the room
    PokerTable* pokerTable = new PokerTable({5.0f, 1.0f, 0.0f}, {4.0f, 0.2f, 2.5f}, BROWN, &physics);
    pokerTable->isDynamicallyAllocated = true;
    dom.AddObject(pokerTable);

    // Create 3 enemies at different positions in the room
    Enemy* enemy1 = new Enemy({-5.0f, 0.0f, 5.0f}, "Shadow");
    enemy1->isDynamicallyAllocated = true;
    dom.AddObject(enemy1);

    Enemy* enemy2 = new Enemy({5.0f, 0.0f, -5.0f}, "Phantom");
    enemy2->isDynamicallyAllocated = true;
    dom.AddObject(enemy2);

    Enemy* enemy3 = new Enemy({-3.0f, 0.0f, -7.0f}, "Wraith");
    enemy3->isDynamicallyAllocated = true;
    dom.AddObject(enemy3);

    // Create spawners inside the room
    Spawner cardSpawner({0.0f, 2.0f, 3.0f}, 2.0f);
    Spawner chipSpawner({-5.0f, 2.0f, -3.0f}, 1.5f);
    Spawner pistolSpawner({3.0f, 2.0f, -5.0f}, 1.0f);

    // Spawn some cards (spawner allocates and adds to DOM)
    cardSpawner.SpawnCards(SUIT_SPADES, RANK_ACE, 3, &physics, &dom);
    cardSpawner.SpawnCards(SUIT_HEARTS, RANK_KING, 2, &physics, &dom);

    // Spawn some chips (spawner allocates and adds to DOM)
    chipSpawner.SpawnChips(1, 5, &physics, &dom);
    chipSpawner.SpawnChips(5, 5, &physics, &dom);
    chipSpawner.SpawnChips(10, 5, &physics, &dom);
    chipSpawner.SpawnChips(25, 5, &physics, &dom);
    chipSpawner.SpawnChips(100, 5, &physics, &dom);

    // Spawn a pistol
    pistolSpawner.SpawnPistols(1, &physics, &dom);

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
        player->Update(deltaTime);

        // Update camera position in lighting shader
        Camera3D* cam = player->GetCamera();
        LightSource::UpdateCameraPosition(cam->position);

        // Update all light sources in shader
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            // Check if object is a LightSource (use dynamic_cast or type check)
            const char* type = obj->GetType();
            if (strcmp(type, "light_bulb") == 0) {
                LightSource* light = static_cast<LightSource*>(obj);
                light->UpdateLight();
            }
        }

        // Step physics simulation
        physics.Step(deltaTime);

        // Update all objects in DOM (except player - already updated above)
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            if (obj != nullptr && obj != player) {  // Skip player - already updated
                obj->Update(deltaTime);
            }
        }

        // Remove inactive dynamically allocated objects from DOM
        for (int i = dom.GetCount() - 1; i >= 0; i--) {
            Object* obj = dom.GetObject(i);
            if (obj != nullptr && !obj->isActive && obj->isDynamicallyAllocated) {
                dom.RemoveObject(obj);
                delete obj;
            }
        }

        // Get closest interactable for E prompt
        Interactable* closestInteractable = player->GetClosestInteractable();

        // Draw
        BeginDrawing();
            ClearBackground(BLACK);

            Camera3D* camera = player->GetCamera();

            // === 3D RENDERING PASS ===
            BeginMode3D(*camera);
                // Apply lighting shader to all 3D objects automatically (except light sources, chips, and closest interactable)
                BeginShaderMode(LightSource::GetLightingShader());
                    // Draw all objects except light sources, chips, and closest interactable
                    for (int i = 0; i < dom.GetCount(); i++) {
                        Object* obj = dom.GetObject(i);
                        if (obj != nullptr) {
                            const char* type = obj->GetType();
                            // Skip light sources and chips - they don't have proper normals for lighting
                            if (strcmp(type, "light_bulb") == 0) continue;
                            if (strncmp(type, "chip_", 5) == 0) continue;
                            // Skip closest interactable only if it exists and matches
                            if (closestInteractable != nullptr && obj == closestInteractable) continue;
                            
                            obj->Draw(*camera);
                        }
                    }
                EndShaderMode();
                
                // Draw objects without shader (light sources and chips)
                for (int i = 0; i < dom.GetCount(); i++) {
                    Object* obj = dom.GetObject(i);
                    if (obj != nullptr && obj != closestInteractable) {
                        const char* type = obj->GetType();
                        if (strcmp(type, "light_bulb") == 0 || strncmp(type, "chip_", 5) == 0) {
                            obj->Draw(*camera);
                        }
                    }
                }
            EndMode3D();

            // Draw E prompt and closest interactable without shader (so it's bright/visible)
            if (closestInteractable != nullptr) {
                BeginMode3D(*camera);
                    // Draw the closest interactable without lighting shader (unaffected, fully bright)
                    closestInteractable->Draw(*camera);
                    
                    // Draw E prompt
                    closestInteractable->DrawPrompt(*camera);
                EndMode3D();
            }

            // Draw held item (pistol) in 3D space - disable depth test so it draws on top
            rlDisableDepthTest();
            BeginMode3D(*camera);
                // Apply lighting shader to held item
                BeginShaderMode(LightSource::GetLightingShader());
                    player->DrawHeldItem();
                EndShaderMode();
            EndMode3D();
            rlEnableDepthTest();

            // === 2D UI RENDERING PASS ===
            // Flush 3D rendering and switch to pure 2D mode
            rlDrawRenderBatchActive();
            rlDisableDepthTest();
            rlDisableBackfaceCulling();

            // Set 2D mode explicitly
            BeginMode2D({{0, 0}, {0, 0}, 0, 1.0f});
                player->DrawInventoryUI();
            EndMode2D();

            // Draw FPS counter
            DrawFPS(10, screenHeight - 30);

            rlEnableDepthTest();
        EndDrawing();
    }

    // De-Initialization
    // Clean up all dynamically allocated objects in DOM
    for (int i = 0; i < dom.GetCount(); i++) {
        Object* obj = dom.GetObject(i);
        // Only delete if it was dynamically allocated
        if (obj->isDynamicallyAllocated) {
            delete obj;
        }
    }

    // Delete stack-allocated objects that were added to DOM
    delete player;
    delete groundFloor;
    delete ceiling;
    delete wallNorth;
    delete wallSouth;
    delete wallEast;
    delete wallWest;
    delete lightBulb;

    // Cleanup lighting system
    LightSource::CleanupLightingSystem();

    CloseWindow();

    return 0;
}
