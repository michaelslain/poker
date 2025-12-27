#include "raylib.h"
#include "raymath.h"
#include <string>
#include <vector>
#include <algorithm>
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
#include "gameplay/room_visibility_manager.hpp"
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

// Light distance struct for sorting lights by distance from camera
struct LightDistance {
    int lightIndex;      // Index in lights array
    float distance;      // Distance from camera
    Light* lightPtr;     // Pointer to light object
};

// Select the N nearest lights based on camera position for rendering
// This reduces shader cost by only computing nearby lights
std::vector<int> SelectNearestLights(const DOM& dom, const Camera3D& camera, int maxLights) {
    std::vector<LightDistance> lightDistances;

    // Collect all lights with their distances from camera
    for (int i = 0; i < dom.GetCount(); i++) {
        Object* obj = dom.GetObject(i);
        if (TypeContains(obj->GetType(), "light")) {
            Light* light = static_cast<Light*>(obj);
            float dist = Vector3Distance(camera.position, light->position);
            lightDistances.push_back({i, dist, light});
        }
    }

    // Sort lights by distance (nearest first)
    std::sort(lightDistances.begin(), lightDistances.end(),
              [](const LightDistance& a, const LightDistance& b) {
                  return a.distance < b.distance;
              });

    // Take only the nearest N lights
    std::vector<int> result;
    int count = std::min((int)lightDistances.size(), maxLights);
    for (int i = 0; i < count; i++) {
        result.push_back(i);  // Store index in lights array (0, 1, 2, ...)
    }

    return result;
}

// Helper function to clean up all DOM objects except player
void CleanupLevel(DOM& dom, Player* player) {
    // Unseat player if seated (tables will be deleted)
    if (player && player->IsSeated()) {
        player->StandUp();
        TraceLog(LOG_INFO, "CLEANUP: Player was seated, standing up before level cleanup");
    }

    // CRITICAL: Freeze player physics during cleanup to prevent falling
    // When floors are deleted, player would fall before new level generates
    dBodyID playerBody = player ? player->GetBody() : nullptr;
    if (playerBody) {
        // Disable gravity and zero out velocities
        dBodySetGravityMode(playerBody, 0);
        dBodySetLinearVel(playerBody, 0, 0, 0);
        dBodySetAngularVel(playerBody, 0, 0, 0);
        TraceLog(LOG_INFO, "CLEANUP: Froze player physics during level cleanup");
    }

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

    // NOTE: Gravity will be re-enabled automatically when GenerateLevel() calls player->Teleport()
    // See Player::Teleport() which does: dBodySetGravityMode(body, 1)
}

// Helper function to generate a level
void GenerateLevel(int levelNum, LevelGenerator& levelGen, HospitalScene& hospital, DOM& dom, Player* player) {
    // NOTE: Do NOT trigger come-down here - that happens when exiting alternate dimension
    // Salvia trip should persist for entire alternate dimension level

    // If in alternate dimension, ALWAYS use procedural generation (even for level 0)
    // This ensures Salvia dimensions have MAX_ROOMS (32) regardless of base level
    if (LevelManager::GetInstance()->IsInAlternateDimension()) {
        levelGen.GenerateLevel(levelNum);
        if (player) {
            player->Teleport(levelGen.GetPlayerSpawnPosition());
        }
    } else if (levelNum == 0) {
        // Normal level 0: Hospital scene
        hospital.Generate();
        if (player) {
            player->Teleport(hospital.GetPlayerSpawnPosition());
        }
    } else {
        // Normal procedurally generated casino level
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

    // Create level generators and visibility manager
    RoomVisibilityManager roomVisibility;
    LevelGenerator levelGenerator(&physics, &dom);
    levelGenerator.SetVisibilityManager(&roomVisibility);
    HospitalScene hospitalScene(&physics, &dom);

    // Initialize scene manager for death scene
    SceneManager* sceneManager = SceneManager::GetInstance();
    sceneManager->RegisterSceneFactory("death", CreateDeathScene);

    // Track state
    bool isInDeathScene = false;
    int previousDimension = 0;  // Track dimension changes for regeneration
    bool isGeneratingLevel = false;  // Track async level generation

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

        #ifdef DEBUG_HOTKEYS
        // DEBUG: Jump to level with number keys (Ctrl+1-9)
        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && player && !isInDeathScene) {
            int targetLevel = -1;
            if (IsKeyPressed(KEY_ONE)) targetLevel = 1;
            else if (IsKeyPressed(KEY_TWO)) targetLevel = 2;
            else if (IsKeyPressed(KEY_THREE)) targetLevel = 3;
            else if (IsKeyPressed(KEY_FOUR)) targetLevel = 4;
            else if (IsKeyPressed(KEY_FIVE)) targetLevel = 5;
            else if (IsKeyPressed(KEY_SIX)) targetLevel = 6;
            else if (IsKeyPressed(KEY_SEVEN)) targetLevel = 7;
            else if (IsKeyPressed(KEY_EIGHT)) targetLevel = 8;
            else if (IsKeyPressed(KEY_NINE)) targetLevel = 9;

            if (targetLevel != -1) {
                TraceLog(LOG_INFO, "DEBUG: Jumping to level %d", targetLevel);
                levelManager->JumpToLevel(targetLevel);
                CleanupLevel(dom, player);

                // Debug uses synchronous generation for instant results
                if (targetLevel == 0) {
                    hospitalScene.Generate();
                    if (player) player->Teleport(hospitalScene.GetPlayerSpawnPosition());
                } else {
                    levelGenerator.GenerateLevel(targetLevel);  // Old synchronous method
                    if (player) player->Teleport(levelGenerator.GetPlayerSpawnPosition());
                }
            }
        }
        #endif

        // Check for stairs collision BEFORE physics/update - trigger level transition
        // IMPORTANT: Must happen before physics.Step() to prevent player from falling between frames
        bool stairsTransitionTriggered = false;
        if (player && !isInDeathScene) {
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

                            // Trigger Salvia come-down when exiting alternate dimension
                            if (PsychedelicManager::IsTripping() && PsychedelicManager::GetTripType() == TripType::SALVIA) {
                                PsychedelicManager::TriggerComeDown();
                                TraceLog(LOG_INFO, "SALVIA: Triggering come-down (exiting alternate dimension)");
                            }

                            TraceLog(LOG_INFO, "STAIRS: Exited alternate dimension, jumped %d levels to level %d",
                                    levelJump, levelManager->GetCurrentLevel());
                        } else {
                            // Normal progression - go to next level
                            levelManager->NextLevel();
                            TraceLog(LOG_INFO, "STAIRS: Progressing to level %d", levelManager->GetCurrentLevel());
                        }

                        stairsTransitionTriggered = true;
                        break;  // Stop iterating - we'll clean up and regenerate
                    }
                }
            }

            // Handle transition immediately - skip rest of frame
            if (stairsTransitionTriggered) {
                // Clean up current level (except player)
                CleanupLevel(dom, player);

                // Start async level generation
                int newLevel = levelManager->GetCurrentLevel();
                if (newLevel == 0) {
                    // Hospital uses synchronous generation (simple scene)
                    hospitalScene.Generate();
                    if (player) {
                        player->Teleport(hospitalScene.GetPlayerSpawnPosition());
                    }
                } else {
                    // Casino uses async generation
                    levelGenerator.StartGeneration(newLevel);
                    isGeneratingLevel = true;
                    TraceLog(LOG_INFO, "STAIRS: Started async generation for level %d", newLevel);
                }

                // Skip physics and updates for this frame - fresh start next frame
                continue;
            }
        }

        // Light culling: Select 8 nearest lights for shader (reduces rendering cost)
        if (player && !isInDeathScene) {
            Camera3D* cam = player->GetCamera();
            std::vector<int> activeLights = SelectNearestLights(dom, *cam, 8);
            LightingManager::SetActiveLights(activeLights);
        }

        // Continue async level generation if active
        if (isGeneratingLevel && player && !isInDeathScene) {
            bool complete = levelGenerator.ContinueGeneration(2);  // 2 rooms per frame

            if (complete) {
                // Generation finished - teleport player to spawn
                player->Teleport(levelGenerator.GetPlayerSpawnPosition());
                isGeneratingLevel = false;
                TraceLog(LOG_INFO, "ASYNC_GEN: Level generation complete, player teleported");
            }
            // If not complete, continue next frame
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

                // Start async regeneration
                int currentLevel = levelManager->GetCurrentLevel();
                if (LevelManager::GetInstance()->IsInAlternateDimension()) {
                    // Salvia dimension - async generation
                    levelGenerator.StartGeneration(currentLevel);
                    isGeneratingLevel = true;
                } else {
                    // Shouldn't happen, but handle it
                    if (currentLevel == 0) {
                        hospitalScene.Generate();
                        if (player) player->Teleport(hospitalScene.GetPlayerSpawnPosition());
                    } else {
                        levelGenerator.StartGeneration(currentLevel);
                        isGeneratingLevel = true;
                    }
                }

                TraceLog(LOG_INFO, "SALVIA: Started async generation for dimension %d", currentDimension);
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

            // Draw objects with lighting (frustum culling only for now)
            if (lightingShader.id != 0) {
                BeginShaderMode(lightingShader);
                for (int i = 0; i < dom.GetCount(); i++) {
                    Object* obj = dom.GetObject(i);
                    if (!obj->usesLighting) continue;

                    // Frustum culling: skip objects outside camera view
                    if (!IsInFrustum(*camera, obj->position, 10.0f)) continue;

                    obj->Draw(*camera);
                }
                EndShaderMode();
            }

            // Draw unlit objects (frustum culling only for now)
            for (int i = 0; i < dom.GetCount(); i++) {
                Object* obj = dom.GetObject(i);
                if (obj->usesLighting) continue;

                // Frustum culling: skip objects outside camera view
                if (!IsInFrustum(*camera, obj->position, 10.0f)) continue;

                obj->Draw(*camera);
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
                float pinkHue = (PsychedelicManager::GetTripType() == TripType::MOLLY) ? intensity : 0.0f;
                int tripType = (int)PsychedelicManager::GetTripType();
                SetShaderValue(psychShader, GetShaderLocation(psychShader, "time"), &tripTime, SHADER_UNIFORM_FLOAT);
                SetShaderValue(psychShader, GetShaderLocation(psychShader, "intensity"), &intensity, SHADER_UNIFORM_FLOAT);
                SetShaderValue(psychShader, GetShaderLocation(psychShader, "pinkHue"), &pinkHue, SHADER_UNIFORM_FLOAT);
                SetShaderValue(psychShader, GetShaderLocation(psychShader, "tripType"), &tripType, SHADER_UNIFORM_INT);

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

            // Draw player coordinates (above FPS)
            Vector3 playerPos = player->position;
            DrawText(TextFormat("X: %.2f  Y: %.2f  Z: %.2f", playerPos.x, playerPos.y, playerPos.z),
                     10, screenHeight - 60, 20, LIME);

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
