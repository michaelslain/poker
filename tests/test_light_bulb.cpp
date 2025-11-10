#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/light_bulb.hpp"

TEST_CASE("LightBulb - Construction", "[light_bulb]") {
    SECTION("Create light bulb with color") {
        LightBulb bulb({0, 5, 0}, YELLOW);
        REQUIRE(bulb.position.x == 0.0f);
        REQUIRE(bulb.position.y == 5.0f);
        REQUIRE(bulb.position.z == 0.0f);
    }
}

TEST_CASE("LightBulb - GetType", "[light_bulb]") {
    LightBulb bulb({0, 5, 0}, WHITE);
    REQUIRE(bulb.GetType().find("light_bulb") != std::string::npos);
}

TEST_CASE("LightSource - Shader Initialization", "[light_bulb][shader][integration]") {
    SECTION("InitLightingSystem loads shader") {
        LightSource::InitLightingSystem();
        
        // Verify shader was loaded (id should be non-zero)
        Shader& shader = LightSource::GetLightingShader();
        REQUIRE(shader.id != 0);
        
        LightSource::CleanupLightingSystem();
    }
    
    SECTION("GetLightingShader returns by reference") {
        LightSource::InitLightingSystem();
        
        Shader& shader1 = LightSource::GetLightingShader();
        Shader& shader2 = LightSource::GetLightingShader();
        
        // Both references should point to the same shader
        REQUIRE(&shader1 == &shader2);
        
        LightSource::CleanupLightingSystem();
    }
    
    SECTION("Multiple init calls are safe") {
        LightSource::InitLightingSystem();
        unsigned int firstID = LightSource::GetLightingShader().id;
        
        // Calling init again should not reload shader
        LightSource::InitLightingSystem();
        REQUIRE(LightSource::GetLightingShader().id == firstID);
        
        LightSource::CleanupLightingSystem();
    }
}

TEST_CASE("LightBulb - UpdateLight integration", "[light_bulb][integration]") {
    SECTION("UpdateLight works with initialized shader") {
        LightSource::InitLightingSystem();
        
        LightBulb bulb({0, 5, 0}, YELLOW);
        
        // This should not crash
        REQUIRE_NOTHROW(bulb.UpdateLight());
        
        LightSource::CleanupLightingSystem();
    }
}
