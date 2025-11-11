#include "scene_manager.hpp"
#include "scenes/game_scene.hpp"
#include "dom.hpp"
#include "physics.hpp"
#include "player.hpp"
#include "light.hpp"
#include "raylib.h"
#include <string>

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
    LightSource::InitLightingSystem();

    // Initialize DOM (main owns this)
    DOM dom;
    DOM::SetGlobal(&dom);

    // Initialize scene manager and register scenes
    SceneManager* sceneManager = SceneManager::GetInstance();
    sceneManager->RegisterSceneFactory("game", CreateGameScene);

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
            LightSource::UpdateCameraPosition(cam->position);
        }

        // Update all light sources
        for (int i = 0; i < dom.GetCount(); i++) {
            Object* obj = dom.GetObject(i);
            if (TypeContains(obj->GetType(), "light_bulb")) {
                LightSource* light = static_cast<LightSource*>(obj);
                light->UpdateLight();
            }
        }

        // Update all objects
        for (int i = 0; i < dom.GetCount(); i++) {
            dom.GetObject(i)->Update(deltaTime);
        }

        // Get closest interactable
        Interactable* closestInteractable = player ? player->GetClosestInteractable() : nullptr;

        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);

        if (player) {
            Camera3D* camera = player->GetCamera();

            BeginMode3D(*camera);

            // Get lighting shader
            Shader& lightingShader = LightSource::GetLightingShader();

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

            // Draw UI
            player->DrawInventoryUI();
            player->DrawBettingUI();
            player->DrawInsanityMeter();
        }

        DrawFPS(10, screenHeight - 30);
        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < dom.GetCount(); i++) {
        delete dom.GetObject(i);
    }
    dom.Cleanup();

    LightSource::CleanupLightingSystem();
    SceneManager::DestroyInstance();

    CloseWindow();
    return 0;
}
