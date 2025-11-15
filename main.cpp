#include "core/dom.hpp"
#include "core/physics.hpp"
#include "entities/player.hpp"
#include "rendering/light.hpp"
#include "rendering/lighting_manager.hpp"
#include "rendering/psychedelic_manager.hpp"
#include "items/interactable.hpp"
#include "core/scene.hpp"
#include "core/scene_manager.hpp"
#include "scenes/game_scene.hpp"
#include "raylib.h"
#include <string>

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

int main(void)
{
    // Initialization
    const int screenWidth = 1500;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Poker - First Person");
    SetTraceLogLevel(LOG_WARNING);
    DisableCursor();

    // Initialize core systems
    PhysicsWorld physics;
    LightingManager::InitLightingSystem();
    PsychedelicManager::InitPsychedelicSystem();

    // Initialize DOM (main owns this)
    DOM dom;
    DOM::SetGlobal(&dom);

    // Initialize scene manager and register scenes
    SceneManager* sceneManager = SceneManager::GetInstance();
    sceneManager->RegisterSceneFactory("game", CreateGameScene);
    sceneManager->RegisterSceneFactory("death", CreateDeathScene);

    // Load initial scene
    Scene* currentScene = sceneManager->CreateScene("game", &physics);
    if (currentScene) {
        // Add all initial objects to DOM
        for (Object* obj : currentScene->GetInitialObjects()) {
            dom.AddObject(obj);
        }
    }

    // Get player reference for rendering
    Player* player = nullptr;
    for (int i = 0; i < dom.GetCount(); i++) {
        if (TypeContains(dom.GetObject(i)->GetType(), "player")) {
            player = static_cast<Player*>(dom.GetObject(i));
            break;
        }
    }

    // Create render texture for psychedelic post-processing
    RenderTexture2D renderTarget = LoadRenderTexture(screenWidth, screenHeight);

    SetTargetFPS(60);

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

        // Get closest interactable (before potential scene switch)
        Interactable* closestInteractable = player ? player->GetClosestInteractable() : nullptr;

        // Rendering
        if (player) {
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
            
            // Draw death vignette on top of everything
            player->insanityManager.DrawDeathVignette();
            
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
        if (player && player->IsDead()) {
            TraceLog(LOG_INFO, "DEATH: Player died, switching to death scene");
            
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
            currentScene = sceneManager->CreateScene("death", &physics);
            if (currentScene) {
                for (Object* obj : currentScene->GetInitialObjects()) {
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

    PsychedelicManager::CleanupPsychedelicSystem();
    LightingManager::CleanupLightingSystem();
    SceneManager::DestroyInstance();

    CloseWindow();

    return 0;
}
