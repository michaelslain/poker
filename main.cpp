#include "raylib.h"
#include <string>
#include "entities/player.hpp"
#include "rendering/light.hpp"
#include "rendering/lighting_manager.hpp"
#include "rendering/psychedelic_manager.hpp"
#include "rendering/render_utils.hpp"
#include "items/interactable.hpp"
#include "core/scene.hpp"
#include "core/scene_manager.hpp"
#include "core/level_manager.hpp"
#include "gameplay/level_generator.hpp"
#include "scenes/hospital_scene.hpp"
#include "world/stairs.hpp"

// Forward declaration of death scene factory
Scene* CreateDeathScene(PhysicsWorld* physics);

// Global debug flag
bool g_showCollisionDebug = false;

// Helper function to check if a type hierarchy contains a component
inline bool TypeContains(const std::string& type, const std::string& component) {
    return type.find("_" + component) != std::string::npos ||
           type.find(component + "_") != std::string::npos ||
           type == component;
}

// Helper function to clean up all DOM objects except player
void CleanupLevel(DOM& dom, Player* player) {
    // Delete all objects except player
    for (int i = 0; i < dom.GetCount(); i++) {
        Object* obj = dom.GetObject(i);
        if (obj != player) {
            delete obj;
        }
    }
    
    // Clear DOM and re-add player
    dom.Cleanup();
    if (player) {
        dom.AddObject(player);
    }
    
    // Reset lighting system for new level
    LightingManager::ResetLights();
}

// Helper function to generate a level
void GenerateLevel(int levelNum, LevelGenerator& levelGen, HospitalScene& hospital, DOM& dom, Player* player) {
    if (levelNum == 0) {
        // Hospital scene
        hospital.Generate();
        if (player) {
            player->Teleport(hospital.GetPlayerSpawnPosition());
        }
    } else {
        // Procedurally generated casino level
        levelGen.GenerateLevel(levelNum);
        if (player) {
            player->Teleport(levelGen.GetPlayerSpawnPosition());
        }
    }
}

int main(void)
{
    // Initialization
    const int screenWidth = 1500;
    const int screenHeight = 900;
    
    InitWindow(screenWidth, screenHeight, "Poker - First Person");
    SetTraceLogLevel(LOG_INFO);  // Temporarily enable INFO logs for debugging
    DisableCursor();
    
    // Initialize core systems
    PhysicsWorld physics;
    LightingManager::InitLightingSystem();
    PsychedelicManager::InitPsychedelicSystem();
    
    // Initialize DOM (main owns this)
    DOM dom;
    DOM::SetGlobal(&dom);
    
    // Set global physics for spawners
    PhysicsWorld::SetGlobal(&physics);
    
    // Initialize level manager
    LevelManager* levelManager = LevelManager::GetInstance();
    levelManager->SetLevel(0);  // Start at hospital (level 0)
    
    // Create level generators
    LevelGenerator levelGenerator(&physics, &dom);
    HospitalScene hospitalScene(&physics, &dom);
    
    // Initialize scene manager for death scene
    SceneManager* sceneManager = SceneManager::GetInstance();
    sceneManager->RegisterSceneFactory("death", CreateDeathScene);
    
    // Track state
    bool isInDeathScene = false;
    int previousDimension = 0;  // Track dimension changes for regeneration
    
    // Generate initial level (hospital)
    hospitalScene.Generate();
    
    // Create player at spawn position
    Vector3 spawnPos = hospitalScene.GetPlayerSpawnPosition();
    Player* player = new Player(spawnPos);
    Player::SetGlobal(player);  // Set global player for substances to access
    dom.AddObject(player);
    
    // Create render texture for psychedelic post-processing
    RenderTexture2D renderTarget = LoadRenderTexture(screenWidth, screenHeight);
    
    // Set FPS to match monitor refresh rate
    int monitorRefreshRate = GetMonitorRefreshRate(GetCurrentMonitor());
    SetTargetFPS(monitorRefreshRate);
    TraceLog(LOG_INFO, "Set target FPS to monitor refresh rate: %d", monitorRefreshRate);
    
    // Main game loop
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        // Toggle cursor with U key
        if (IsKeyPressed(KEY_U)) {
            if (IsCursorHidden()) EnableCursor();
            else DisableCursor();
        }
        
        // Toggle collision debug with G key
        if (IsKeyPressed(KEY_G)) {
            g_showCollisionDebug = !g_showCollisionDebug;
            TraceLog(LOG_INFO, "Collision debug: %s", g_showCollisionDebug ? "ON" : "OFF");
        }
        
        // Update physics
        physics.Step(deltaTime);
        
        // Update camera in lighting shader
        if (player) {
            Camera3D* cam = player->GetCamera();
            LightingManager::UpdateCameraPosition(cam->position);
        }
        
        // Update all light sources
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            if (TypeContains(obj->GetType(), "light")) {
                Light* light = static_cast<Light*>(obj);
                light->UpdateLight();
            }
        }
        
        // Update all objects
        for (int i = 0; i < dom.GetCount(); i++) {
            dom.GetObject(i)->Update(deltaTime);
        }
        
        // Update psychedelic effect
        PsychedelicManager::Update(deltaTime);
        
        // Check for dimension change (salvia consumed) - regenerate level
        if (player && !isInDeathScene) {
            int currentDimension = levelManager->GetCurrentDimension();
            if (currentDimension != previousDimension) {
                TraceLog(LOG_INFO, "SALVIA: Dimension changed from %d to %d, regenerating level", previousDimension, currentDimension);
                previousDimension = currentDimension;
                
                // Clean up current level (except player)
                CleanupLevel(dom, player);
                
                // Regenerate current level in new dimension
                int currentLevel = levelManager->GetCurrentLevel();
                GenerateLevel(currentLevel, levelGenerator, hospitalScene, dom, player);
                
                TraceLog(LOG_INFO, "SALVIA: Level %d regenerated in dimension %d", currentLevel, currentDimension);
            }
        }
        
        // Check for stairs collision - trigger level transition
        if (player && !isInDeathScene) {
            bool transitionTriggered = false;
            for (int i = 0; i < dom.GetCount(); i++) {
                Object* obj = dom.GetObject(i);
                if (TypeContains(obj->GetType(), "stairs")) {
                    Stairs* stairs = static_cast<Stairs*>(obj);
                    
                    // Check collision with player
                    if (stairs->CheckPlayerCollision(player->GetGeom())) {
                        TraceLog(LOG_INFO, "STAIRS: Player reached stairs, transitioning level");
                        
                        // If in alternate dimension, exit with random jump
                        if (levelManager->IsInAlternateDimension()) {
                            int levelJump = levelManager->GenerateRandomLevelJump();
                            levelManager->ExitAlternateDimension(levelJump);
                            previousDimension = 0;  // Back to normal dimension
                            TraceLog(LOG_INFO, "STAIRS: Exited alternate dimension, jumped %d levels to level %d", 
                                    levelJump, levelManager->GetCurrentLevel());
                        } else {
                            // Normal progression - go to next level
                            levelManager->NextLevel();
                            TraceLog(LOG_INFO, "STAIRS: Progressing to level %d", levelManager->GetCurrentLevel());
                        }
                        
                        transitionTriggered = true;
                        break;  // Stop iterating - we'll clean up and regenerate after the loop
                    }
                }
            }
            
            // Handle transition after finishing DOM iteration
            if (transitionTriggered) {
                // Clean up current level (except player)
                CleanupLevel(dom, player);
                
                // Generate new level
                int newLevel = levelManager->GetCurrentLevel();
                GenerateLevel(newLevel, levelGenerator, hospitalScene, dom, player);
                
                TraceLog(LOG_INFO, "STAIRS: New level %d loaded", newLevel);
            }
        }
        
        // Get closest interactable (before potential scene switch)
        Interactable* closestInteractable = player ? player->GetClosestInteractable() : nullptr;
        
        // Rendering
        if (player && !isInDeathScene) {
            Camera3D* camera = player->GetCamera();
            
            // Step 1: Render 3D scene to texture
            BeginTextureMode(renderTarget);
            ClearBackground(BLACK);
            
            BeginMode3D(*camera);
            
            // Get lighting shader
            Shader& lightingShader = LightingManager::GetLightingShader();
            
            // Draw objects with lighting
            if (lightingShader.id != 0) {
                BeginShaderMode(lightingShader);
                for (int i = 0; i < dom.GetCount(); i++) {
                    Object* obj = dom.GetObject(i);
                    // Skip unlit objects
                    if (!obj->usesLighting) continue;
                    obj->Draw(*camera);
                }
                EndShaderMode();
            }
            
            // Draw unlit objects
            for (int i = 0; i < dom.GetCount(); i++) {
                Object* obj = dom.GetObject(i);
                if (!obj->usesLighting) {
                    obj->Draw(*camera);
                }
            }
            
            // Draw held item (needs to be in 3D mode)
            player->DrawHeldItem();
            
            // Draw closest interactable prompt
            if (closestInteractable) {
                closestInteractable->DrawPrompt(*camera);
            }
            
            EndMode3D();
            EndTextureMode();
            
            // Step 2: Draw to screen with optional psychedelic shader
            BeginDrawing();
            ClearBackground(BLACK);
            
            // Apply psychedelic shader if tripping
            if (PsychedelicManager::IsTripping()) {
                Shader& psychShader = PsychedelicManager::GetPsychedelicShader();
                
                // Update shader uniforms
                float tripTime = PsychedelicManager::GetTripTime();
                float intensity = PsychedelicManager::GetCurrentIntensity();
                SetShaderValue(psychShader, GetShaderLocation(psychShader, "time"), &tripTime, SHADER_UNIFORM_FLOAT);
                SetShaderValue(psychShader, GetShaderLocation(psychShader, "intensity"), &intensity, SHADER_UNIFORM_FLOAT);
                
                BeginShaderMode(psychShader);
            }
            
            // Draw the render texture to screen
            DrawTextureRec(renderTarget.texture,
                          (Rectangle){ 0, 0, (float)renderTarget.texture.width, -(float)renderTarget.texture.height },
                          (Vector2){ 0, 0 }, WHITE);
            
            if (PsychedelicManager::IsTripping()) {
                EndShaderMode();
            }
            
            // Draw UI on top (not affected by psychedelic shader)
            player->DrawInventoryUI();
            player->DrawBettingUI();
            player->insanityManager.DrawMeter();
            
            // Draw level counter
            DrawLevelUI(levelManager->GetCurrentLevel(), levelManager->GetCurrentDimension());
            
            // Draw death vignette on top of everything
            player->DrawDeathVignette();
            
            DrawFPS(10, screenHeight - 30);
            
            EndDrawing();
        } else {
            // No player - render death scene or other non-player scenes
            BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw death scene objects (they handle their own 2D rendering)
            for (int i = 0; i < dom.GetCount(); i++) {
                Object* obj = dom.GetObject(i);
                // Death scene objects draw in 2D, so we pass a dummy camera
                Camera3D dummyCamera = {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, 0, 0};
                obj->Draw(dummyCamera);
            }
            
            DrawFPS(10, screenHeight - 30);
            
            EndDrawing();
        }
        
        // Check if player has died from insanity (AFTER rendering)
        if (player && player->IsDead() && !isInDeathScene) {
            TraceLog(LOG_INFO, "DEATH: Player died, switching to death scene");
            
            isInDeathScene = true;
            
            // Clear player reference FIRST before cleanup
            player = nullptr;
            closestInteractable = nullptr;
            
            // Clean up current scene objects
            for (int i = 0; i < dom.GetCount(); i++) {
                Object* obj = dom.GetObject(i);
                delete obj;
            }
            dom.Cleanup();
            
            TraceLog(LOG_INFO, "DEATH: Scene cleaned up, creating death scene");
            
            // Switch to death scene
            Scene* deathScene = sceneManager->CreateScene("death", &physics);
            if (deathScene) {
                for (Object* obj : deathScene->GetInitialObjects()) {
                    dom.AddObject(obj);
                }
                TraceLog(LOG_INFO, "DEATH: Death scene loaded successfully");
            }
            
            // Continue to next frame - don't try to update/render with nullptr player
            continue;
        }
    }
    
    // Cleanup
    UnloadRenderTexture(renderTarget);
    
    for (int i = 0; i < dom.GetCount(); i++) {
        delete dom.GetObject(i);
    }
    dom.Cleanup();
    
    LevelManager::Destroy();
    PsychedelicManager::CleanupPsychedelicSystem();
    LightingManager::CleanupLightingSystem();
    SceneManager::DestroyInstance();
    
    CloseWindow();
    
    return 0;
}
