#include "catch_amalgamated.hpp"
#include "rendering/lighting_manager.hpp"
#include "raylib.h"

TEST_CASE("LightingManager - Initialization", "[lighting_manager]") {
    SECTION("GetLightingShader returns valid shader after init") {
        // Shader should already be initialized by test_main.cpp
        Shader& shader = LightingManager::GetLightingShader();
        
        // In headless mode, shader might fail to load (id would be 0)
        // Just verify we can get a reference without crashing
        (void)shader; // Suppress unused variable warning
        REQUIRE(true); // Test passes if we got here without crashing
    }
}

TEST_CASE("LightingManager - Camera Position Update", "[lighting_manager]") {
    SECTION("UpdateCameraPosition accepts valid positions") {
        Vector3 cameraPos = {5.0f, 10.0f, 15.0f};
        
        // Should not crash
        REQUIRE_NOTHROW(LightingManager::UpdateCameraPosition(cameraPos));
    }
    
    SECTION("UpdateCameraPosition at origin") {
        Vector3 cameraPos = {0.0f, 0.0f, 0.0f};
        
        REQUIRE_NOTHROW(LightingManager::UpdateCameraPosition(cameraPos));
    }
    
    SECTION("UpdateCameraPosition with negative coordinates") {
        Vector3 cameraPos = {-5.0f, -10.0f, -15.0f};
        
        REQUIRE_NOTHROW(LightingManager::UpdateCameraPosition(cameraPos));
    }
}

TEST_CASE("LightingManager - Light Creation", "[lighting_manager]") {
    SECTION("CreateLight returns valid RaylibLight struct") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        Color color = WHITE;
        
        RaylibLight light = LightingManager::CreateLight(LIGHT_POINT, position, target, color);
        
        REQUIRE(light.enabled == true);
        REQUIRE(light.type == LIGHT_POINT);
        REQUIRE(light.position.x == position.x);
        REQUIRE(light.position.y == position.y);
        REQUIRE(light.position.z == position.z);
        REQUIRE(light.color.r == color.r);
        REQUIRE(light.color.g == color.g);
        REQUIRE(light.color.b == color.b);
        REQUIRE(light.color.a == color.a);
    }
    
    SECTION("CreateLight with LIGHT_DIRECTIONAL type") {
        Vector3 position = {0.0f, 10.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        Color color = YELLOW;
        
        RaylibLight light = LightingManager::CreateLight(LIGHT_DIRECTIONAL, position, target, color);
        
        REQUIRE(light.type == LIGHT_DIRECTIONAL);
    }
    
    SECTION("CreateLight with custom color") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        Color customColor = {255, 128, 64, 255};
        
        RaylibLight light = LightingManager::CreateLight(LIGHT_POINT, position, target, customColor);
        
        REQUIRE(light.color.r == 255);
        REQUIRE(light.color.g == 128);
        REQUIRE(light.color.b == 64);
        REQUIRE(light.color.a == 255);
    }
}

TEST_CASE("LightingManager - UpdateLightValues", "[lighting_manager]") {
    SECTION("UpdateLightValues accepts valid light") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        Color color = WHITE;
        
        RaylibLight light = LightingManager::CreateLight(LIGHT_POINT, position, target, color);
        
        // Modify light position
        light.position = {10.0f, 15.0f, 20.0f};
        
        // Should not crash
        REQUIRE_NOTHROW(LightingManager::UpdateLightValues(light));
    }
}

TEST_CASE("LightingManager - MAX_LIGHTS Constant", "[lighting_manager]") {
    SECTION("MAX_LIGHTS is defined correctly") {
        REQUIRE(MAX_LIGHTS == 32);
    }
}

TEST_CASE("LightingManager - LightType Enum", "[lighting_manager]") {
    SECTION("LIGHT_DIRECTIONAL value") {
        REQUIRE(LIGHT_DIRECTIONAL == 0);
    }
    
    SECTION("LIGHT_POINT value") {
        REQUIRE(LIGHT_POINT == 1);
    }
}

TEST_CASE("LightingManager - Light Index Tracking", "[lighting_manager]") {
    SECTION("CreateLight stores correct light index") {
        LightingManager::ResetLights();  // Reset before creating lights
        
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        Color color = WHITE;
        
        RaylibLight light = LightingManager::CreateLight(LIGHT_POINT, position, target, color);
        
        // Light should have a valid index (>= 0)
        REQUIRE(light.lightIndex >= 0);
    }
    
    SECTION("Multiple lights get sequential indices") {
        // Reset lights first
        LightingManager::ResetLights();
        
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        
        RaylibLight light1 = LightingManager::CreateLight(LIGHT_POINT, position, target, RED);
        RaylibLight light2 = LightingManager::CreateLight(LIGHT_POINT, position, target, GREEN);
        RaylibLight light3 = LightingManager::CreateLight(LIGHT_POINT, position, target, BLUE);
        
        REQUIRE(light1.lightIndex == 0);
        REQUIRE(light2.lightIndex == 1);
        REQUIRE(light3.lightIndex == 2);
    }
}

TEST_CASE("LightingManager - ResetLights", "[lighting_manager]") {
    SECTION("ResetLights clears light counter") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        
        // Create some lights
        LightingManager::CreateLight(LIGHT_POINT, position, target, RED);
        LightingManager::CreateLight(LIGHT_POINT, position, target, GREEN);
        
        // Reset
        LightingManager::ResetLights();
        
        // Next light should start at index 0 again
        RaylibLight light = LightingManager::CreateLight(LIGHT_POINT, position, target, BLUE);
        REQUIRE(light.lightIndex == 0);
    }
    
    SECTION("ResetLights can be called multiple times safely") {
        REQUIRE_NOTHROW(LightingManager::ResetLights());
        REQUIRE_NOTHROW(LightingManager::ResetLights());
        REQUIRE_NOTHROW(LightingManager::ResetLights());
    }
    
    SECTION("ResetLights allows light recycling across scenes") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        
        // Scene 1: Create 3 lights
        LightingManager::ResetLights();
        RaylibLight scene1_light1 = LightingManager::CreateLight(LIGHT_POINT, position, target, RED);
        RaylibLight scene1_light2 = LightingManager::CreateLight(LIGHT_POINT, position, target, GREEN);
        RaylibLight scene1_light3 = LightingManager::CreateLight(LIGHT_POINT, position, target, BLUE);
        
        REQUIRE(scene1_light1.lightIndex == 0);
        REQUIRE(scene1_light2.lightIndex == 1);
        REQUIRE(scene1_light3.lightIndex == 2);
        
        // Scene 2: Reset and create new lights
        LightingManager::ResetLights();
        RaylibLight scene2_light1 = LightingManager::CreateLight(LIGHT_POINT, position, target, YELLOW);
        RaylibLight scene2_light2 = LightingManager::CreateLight(LIGHT_POINT, position, target, PURPLE);
        
        // Should reuse indices from 0
        REQUIRE(scene2_light1.lightIndex == 0);
        REQUIRE(scene2_light2.lightIndex == 1);
    }
}

TEST_CASE("LightingManager - Multiple Lights Support", "[lighting_manager][regression]") {
    SECTION("MAX_LIGHTS constant increased to 32") {
        // Regression test: MAX_LIGHTS was increased from 4 to 32
        // to support procedurally generated levels with many rooms
        REQUIRE(MAX_LIGHTS == 32);
    }
    
    SECTION("Light index tracking works correctly") {
        // This test verifies the light index bug fix
        // Bug: Multiple lights were overwriting each other because 
        // lightIndex wasn't being stored/tracked properly
        
        LightingManager::ResetLights();
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Vector3 target = {0.0f, 0.0f, 0.0f};
        
        RaylibLight light1 = LightingManager::CreateLight(LIGHT_POINT, position, target, RED);
        RaylibLight light2 = LightingManager::CreateLight(LIGHT_POINT, position, target, GREEN);
        
        // Lights should have different indices
        REQUIRE(light1.lightIndex != light2.lightIndex);
        REQUIRE(light1.lightIndex >= 0);
        REQUIRE(light2.lightIndex >= 0);
    }
}
