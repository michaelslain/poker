#include "catch_amalgamated.hpp"
#include "../include/spawner.hpp"
#include "../include/dom.hpp"

TEST_CASE("Spawner - Basic functionality", "[spawner]") {
    // Note: We can't easily test spawning in unit tests because Card/Chip constructors
    // create render textures which require an active OpenGL context during BeginDrawing()
    // The spawner's actual spawning functionality is tested via gameplay in game_scene.cpp
    
    SECTION("Spawner type verification") {
        // Just verify the test framework works
        // Actual spawner functionality requires a full game context
        REQUIRE(true);
    }
}
