#include "catch_amalgamated.hpp"
#include "rendering/light_bulb.hpp"
#include "rendering/lighting_manager.hpp"
#include "raylib.h"

TEST_CASE("LightBulb - Construction", "[light_bulb]") {
    SECTION("Constructor creates light bulb at position") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Color color = WHITE;
        
        LightBulb bulb(position, color);
        
        REQUIRE(bulb.position.x == 0.0f);
        REQUIRE(bulb.position.y == 5.0f);
        REQUIRE(bulb.position.z == 0.0f);
    }
    
    SECTION("Constructor with custom color") {
        Vector3 position = {0.0f, 5.0f, 0.0f};
        Color customColor = YELLOW;
        
        LightBulb bulb(position, customColor);
        
        // Light bulb should be created without errors
        REQUIRE(bulb.position.y == 5.0f);
    }
    
    SECTION("Light bulb doesn't use lighting") {
        LightBulb bulb({0, 5, 0}, WHITE);
        
        REQUIRE(bulb.usesLighting == false);
    }
}

TEST_CASE("LightBulb - Type System", "[light_bulb]") {
    SECTION("GetType returns hierarchical type") {
        LightBulb bulb({0, 5, 0}, WHITE);
        std::string type = bulb.GetType();
        
        REQUIRE(type.find("object") != std::string::npos);
        REQUIRE(type.find("light") != std::string::npos);
        REQUIRE(type.find("light_bulb") != std::string::npos);
    }
}

TEST_CASE("LightBulb - UpdateLight", "[light_bulb]") {
    SECTION("UpdateLight synchronizes position") {
        LightBulb bulb({0, 5, 0}, WHITE);
        
        // Move the bulb
        bulb.position = {10.0f, 15.0f, 20.0f};
        
        // Update should not crash
        REQUIRE_NOTHROW(bulb.UpdateLight());
    }
}

TEST_CASE("LightBulb - Update", "[light_bulb]") {
    SECTION("Update with deltaTime") {
        LightBulb bulb({0, 5, 0}, WHITE);
        
        REQUIRE_NOTHROW(bulb.Update(0.016f));
    }
    
    SECTION("Update with zero deltaTime") {
        LightBulb bulb({0, 5, 0}, WHITE);
        
        REQUIRE_NOTHROW(bulb.Update(0.0f));
    }
}

TEST_CASE("LightBulb - Draw", "[light_bulb]") {
    SECTION("Draw with camera") {
        LightBulb bulb({0, 5, 0}, WHITE);
        Camera3D camera = {};
        camera.position = {0, 0, -10};
        camera.target = {0, 0, 0};
        camera.up = {0, 1, 0};
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        
        // Draw should not crash (even in headless mode)
        REQUIRE_NOTHROW(bulb.Draw(camera));
    }
}

TEST_CASE("LightBulb - Inheritance", "[light_bulb]") {
    SECTION("LightBulb is a Light") {
        LightBulb bulb({0, 5, 0}, WHITE);
        Light* light = &bulb;
        
        REQUIRE(light != nullptr);
        REQUIRE(light->GetType().find("light_bulb") != std::string::npos);
    }
    
    SECTION("LightBulb is an Object") {
        LightBulb bulb({0, 5, 0}, WHITE);
        Object* obj = &bulb;
        
        REQUIRE(obj != nullptr);
        REQUIRE(obj->GetType().find("light") != std::string::npos);
    }
    
    SECTION("LightBulb has unique ID") {
        LightBulb bulb1({0, 5, 0}, WHITE);
        LightBulb bulb2({1, 6, 1}, YELLOW);
        
        REQUIRE(bulb1.GetID() != bulb2.GetID());
    }
}

TEST_CASE("LightBulb - Memory Management", "[light_bulb]") {
    SECTION("Destructor cleans up properly") {
        // Create bulb in scope
        {
            LightBulb bulb({0, 5, 0}, WHITE);
            REQUIRE(bulb.position.y == 5.0f);
        }
        // Destructor should have been called without crashes
        
        REQUIRE(true); // If we reach here, destructor worked
    }
    
    SECTION("Multiple bulbs can coexist") {
        LightBulb bulb1({0, 5, 0}, WHITE);
        LightBulb bulb2({5, 5, 5}, RED);
        LightBulb bulb3({-5, 5, -5}, BLUE);
        
        REQUIRE(bulb1.GetID() != bulb2.GetID());
        REQUIRE(bulb2.GetID() != bulb3.GetID());
        REQUIRE(bulb1.GetID() != bulb3.GetID());
    }
}

TEST_CASE("LightBulb - Position Changes", "[light_bulb]") {
    SECTION("Position can be modified") {
        LightBulb bulb({0, 5, 0}, WHITE);
        
        bulb.position = {10.0f, 15.0f, 20.0f};
        
        REQUIRE(bulb.position.x == 10.0f);
        REQUIRE(bulb.position.y == 15.0f);
        REQUIRE(bulb.position.z == 20.0f);
    }
    
    SECTION("UpdateLight after position change") {
        LightBulb bulb({0, 5, 0}, WHITE);
        bulb.position = {10.0f, 15.0f, 20.0f};
        
        // Should synchronize internal light position
        REQUIRE_NOTHROW(bulb.UpdateLight());
    }
}
