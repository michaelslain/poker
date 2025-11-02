#include "raylib.h"
#include "rlgl.h"
#include "interactable.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "pistol.hpp"
#include "bullet.hpp"
#include "plane.hpp"
#include "player.hpp"
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
    
    // Initialize DOM
    DOM dom;
    DOM::SetGlobal(&dom);
    
    // Initialize player (using C++ new and constructor)
    Player* player = new Player({0.0f, 0.0f, 0.0f}, &physics);
    dom.AddObject(player);
    
    // Initialize ground plane with physics
    Plane* groundPlane = new Plane({0.0f, 0.0f, 0.0f}, {50.0f, 50.0f}, LIGHTGRAY, &physics);
    dom.AddObject(groundPlane);
    
    // Create poker table
    PokerTable* pokerTable = new PokerTable({5.0f, 1.0f, 0.0f}, {4.0f, 0.2f, 2.5f}, BROWN, &physics);
    pokerTable->isDynamicallyAllocated = true;
    dom.AddObject(pokerTable);
    
    // Create spawners just above the plane
    Spawner cardSpawner({0.0f, 2.0f, 0.0f}, 2.0f);
    Spawner chipSpawner({-5.0f, 2.0f, -5.0f}, 1.5f);
    Spawner pistolSpawner({3.0f, 2.0f, 3.0f}, 1.0f);
    
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
        
        // Step physics simulation
        physics.Step(deltaTime);
        
        // Update all objects in DOM (except player - already updated above)
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            if (obj != nullptr && obj != player) {  // Skip player - already updated
                obj->Update(deltaTime);
            }
        }
        
        // Remove expired bullets from DOM
        for (int i = dom.GetCount() - 1; i >= 0; i--) {
            Object* obj = dom.GetObject(i);
            if (obj != nullptr && strcmp(obj->GetType(), "bullet") == 0) {
                Bullet* bullet = static_cast<Bullet*>(obj);
                if (bullet->IsExpired() || !bullet->isActive) {
                    dom.RemoveObject(bullet);
                    delete bullet;
                }
            }
        }
        
        // Get closest interactable for E prompt
        Interactable* closestInteractable = player->GetClosestInteractable();
        
        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            Camera3D* camera = player->GetCamera();
            
            // === 3D RENDERING PASS ===
            BeginMode3D(*camera);
                // Draw ground plane
                groundPlane->Draw(*camera);
                DrawGrid(50, 1.0f);
                
                // Draw all objects in DOM
                for (int i = 0; i < dom.GetCount(); i++) {
                    Object* obj = dom.GetObject(i);
                    if (obj != nullptr) {
                        obj->Draw(*camera);
                    }
                }
            EndMode3D();
            
            // Draw E prompt above closest interactable
            if (closestInteractable != nullptr) {
                BeginMode3D(*camera);
                    closestInteractable->DrawPrompt(*camera);
                EndMode3D();
            }
            
            // Draw held item (pistol) in 3D space - disable depth test so it draws on top
            rlDisableDepthTest();
            BeginMode3D(*camera);
                player->DrawHeldItem();
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
    delete groundPlane;
    
    CloseWindow();
    
    return 0;
}
