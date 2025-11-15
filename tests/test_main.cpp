#define CATCH_CONFIG_RUNNER
#include "catch_amalgamated.hpp"
#include "raylib.h"
#include "rendering/lighting_manager.hpp"
#include "core/dom.hpp"

// Global variables needed by the game code
bool g_showCollisionDebug = false;

int main(int argc, char* argv[]) {
    // Initialize raylib in headless mode for tests
    SetTraceLogLevel(LOG_NONE); // Disable raylib logs
    InitWindow(800, 600, "Tests");
    SetWindowState(FLAG_WINDOW_HIDDEN); // Hide the window
    
    // Initialize lighting system for tests that need shaders
    LightingManager::InitLightingSystem();
    
    // Initialize global DOM for tests that need it (like PokerTable)
    DOM globalDom;
    DOM::SetGlobal(&globalDom);
    
    int result = Catch::Session().run(argc, argv);
    
    // Cleanup (must cleanup shader BEFORE closing window)
    DOM::SetGlobal(nullptr);
    LightingManager::CleanupLightingSystem();
    CloseWindow();
    
    return result;
}
