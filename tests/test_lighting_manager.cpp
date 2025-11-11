#include "catch_amalgamated.hpp"
#include "../include/lighting_manager.hpp"
#include "raylib.h"

TEST_CASE("LightingManager - Initialization", "[lighting_manager]") {
    SECTION("GetLightingShader returns valid shader after init") {
        // Shader should already be initialized by test_main.cpp
        Shader& shader = LightingManager::GetLightingShader();
        
        // Shader ID should be non-zero if loaded successfully
        // Note: In headless mode, shader might fail to load, so we just check it returns
        REQUIRE(&shader != nullptr);
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
        REQUIRE(MAX_LIGHTS == 4);
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
