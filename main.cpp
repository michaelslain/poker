#include "raylib.h"
#include "rlgl.h"
#include "interactable.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "pistol.hpp"
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

// Global debug flag for collision visualization
bool g_showCollisionDebug = false;

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Poker - First Person");
    
    // Disable raylib's verbose logging (textures, FBOs, etc.)
    SetTraceLogLevel(LOG_WARNING);  // Only show warnings and errors from raylib
    
    // Poker logs will still show because POKER_LOG uses TraceLog directly
    POKER_LOG(LOG_INFO, "=== GAME STARTING ===");

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
    
    // Give player starting chips (500 chips total)
    // Add chips directly to player's inventory
    Inventory* playerInv = player->GetInventory();
    for (int i = 0; i < 5; i++) {  // 5x 100 chips
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);  // No physics, just in inventory
        playerInv->AddItem(chip);
    }
    TraceLog(LOG_INFO, "Player initialized with 500 chips in inventory");

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
    dom.AddObject(pokerTable);

    // Create 3 enemies at different positions in the room
    Enemy* enemy1 = new Enemy({-5.0f, 0.0f, 5.0f}, "Person 1");
    dom.AddObject(enemy1);
    // Give enemy1 starting chips (500 chips)
    Inventory* enemy1Inv = enemy1->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        enemy1Inv->AddItem(chip);
    }

    Enemy* enemy2 = new Enemy({5.0f, 0.0f, -5.0f}, "Person 2");
    dom.AddObject(enemy2);
    // Give enemy2 starting chips (500 chips)
    Inventory* enemy2Inv = enemy2->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        enemy2Inv->AddItem(chip);
    }

    Enemy* enemy3 = new Enemy({-3.0f, 0.0f, -7.0f}, "Person 3");
    dom.AddObject(enemy3);
    // Give enemy3 starting chips (500 chips)
    Inventory* enemy3Inv = enemy3->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        enemy3Inv->AddItem(chip);
    }
    
    // Seat enemies at the poker table
    int seat1 = pokerTable->FindClosestOpenSeat(enemy1->position);
    if (seat1 != -1) {
        pokerTable->SeatPerson(enemy1, seat1);
    }
    
    int seat2 = pokerTable->FindClosestOpenSeat(enemy2->position);
    if (seat2 != -1) {
        pokerTable->SeatPerson(enemy2, seat2);
    }
    
    int seat3 = pokerTable->FindClosestOpenSeat(enemy3->position);
    if (seat3 != -1) {
        pokerTable->SeatPerson(enemy3, seat3);
    }

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
    
    GAME_LOG(LOG_INFO, "About to start game loop. DOM has %d objects:", dom.GetCount());
    for (int i = 0; i < dom.GetCount(); i++) {
        Object* obj = dom.GetObject(i);
        GAME_LOG(LOG_INFO, "  [%d] %s at %p", i, obj ? obj->GetType() : "null", (void*)obj);
    }

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
        
        // Toggle collision debug with G key
        if (IsKeyPressed(KEY_G)) {
            g_showCollisionDebug = !g_showCollisionDebug;
            GAME_LOG(LOG_INFO, "Collision debug: %s", g_showCollisionDebug ? "ON" : "OFF");
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
        static int frameCount = 0;
        frameCount++;
        if (frameCount == 1) {
            GAME_LOG(LOG_INFO, "DOM has %d objects", dom.GetCount());
            for (int i = 0; i < dom.GetCount(); i++) {
                Object* obj = dom.GetObject(i);
                GAME_LOG(LOG_INFO, "  Object %d: %s", i, obj ? obj->GetType() : "null");
            }
        }
        
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            if (obj != nullptr && obj != player) {
                obj->Update(deltaTime);
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
                // Apply lighting shader to all 3D objects automatically (except light sources, chips, enemies, persons, players, and closest interactable)
                BeginShaderMode(LightSource::GetLightingShader());
                    // Draw all root objects (and their children recursively) except light sources, chips, enemies, persons, players, and closest interactable
                    for (int i = 0; i < dom.GetCount(); i++) {
                        Object* obj = dom.GetObject(i);
                        if (obj != nullptr) {
                            const char* type = obj->GetType();
                            // Skip light sources and chips - they don't have proper normals for lighting
                            if (strcmp(type, "light_bulb") == 0) continue;
                            if (strncmp(type, "chip_", 5) == 0) continue;
                            // Skip enemies, persons, and players - they should be pitch black (unaffected by lighting)
                            if (strcmp(type, "enemy") == 0) continue;
                            if (strcmp(type, "person") == 0) continue;
                            if (strcmp(type, "player") == 0) continue;
                            // Skip closest interactable only if it exists and matches
                            if (closestInteractable != nullptr && obj == closestInteractable) continue;
                            
                            obj->Draw(*camera);
                        }
                    }
                EndShaderMode();
                
                // Draw objects without shader (light sources, chips, enemies, persons, and players)
                for (int i = 0; i < dom.GetCount(); i++) {
                    Object* obj = dom.GetObject(i);
                    if (obj != nullptr && obj != closestInteractable) {
                        const char* type = obj->GetType();
                        if (strcmp(type, "light_bulb") == 0 || strncmp(type, "chip_", 5) == 0 || 
                            strcmp(type, "enemy") == 0 || strcmp(type, "person") == 0 || strcmp(type, "player") == 0) {
                            obj->Draw(*camera);
                        }
                    }
                }
            EndMode3D();

            // Draw E prompt and closest interactable without shader (so it's bright/visible)
            if (closestInteractable != nullptr) {
                BeginMode3D(*camera);
                    // Draw the closest interactable without lighting shader (unaffected, fully bright)
                    // Use Draw to also draw any children (e.g., Deck and Dealer for PokerTable)
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
                
                // Draw betting UI (will only show if bettingUIActive is true)
                // DrawBettingUI handles whether to show or not internally
                player->DrawBettingUI();
            EndMode2D();

            // Draw FPS counter
            DrawFPS(10, screenHeight - 30);

            rlEnableDepthTest();
        EndDrawing();
    }

    // De-Initialization
    GAME_LOG(LOG_INFO, "Starting cleanup - %d objects in DOM", dom.GetCount());
    
    // Clean up all objects in DOM
    for (int i = 0; i < dom.GetCount(); i++) {
        Object* obj = dom.GetObject(i);
        GAME_LOG(LOG_INFO, "Deleting object %d: %s", i, obj->GetType());
        delete obj;
        GAME_LOG(LOG_INFO, "Object %d deleted", i);
    }
    
    GAME_LOG(LOG_INFO, "All DOM objects deleted");

    // Cleanup lighting system
    LightSource::CleanupLightingSystem();

    CloseWindow();

    return 0;
}
