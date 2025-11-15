#include "catch_amalgamated.hpp"
#include "rendering/light.hpp"
#include "raylib.h"

TEST_CASE("Light - Construction", "[light]") {
    SECTION("Default constructor creates light at origin") {
        Light light({0, 0, 0});
        
        REQUIRE(light.position.x == 0.0f);
        REQUIRE(light.position.y == 0.0f);
        REQUIRE(light.position.z == 0.0f);
    }
    
    SECTION("Constructor with custom position") {
        Light light({5.0f, 10.0f, 15.0f});
        
        REQUIRE(light.position.x == 5.0f);
        REQUIRE(light.position.y == 10.0f);
        REQUIRE(light.position.z == 15.0f);
    }
    
    SECTION("Light sources don't use lighting") {
        Light light({0, 0, 0});
        
        REQUIRE(light.usesLighting == false);
    }
}

TEST_CASE("Light - Type System", "[light]") {
    SECTION("GetType returns hierarchical type") {
        Light light({0, 0, 0});
        std::string type = light.GetType();
        
        REQUIRE(type.find("object") != std::string::npos);
        REQUIRE(type.find("light") != std::string::npos);
    }
}

TEST_CASE("Light - UpdateLight", "[light]") {
    SECTION("UpdateLight can be called without error") {
        Light light({0, 0, 0});
        
        // Should not crash - base implementation is empty
        REQUIRE_NOTHROW(light.UpdateLight());
    }
}

TEST_CASE("Light - Inheritance", "[light]") {
    SECTION("Light is an Object") {
        Light light({0, 0, 0});
        Object* obj = &light;
        
        REQUIRE(obj != nullptr);
        REQUIRE(obj->GetType().find("light") != std::string::npos);
    }
    
    SECTION("Light has unique ID from Object") {
        Light light1({0, 0, 0});
        Light light2({1, 1, 1});
        
        REQUIRE(light1.GetID() != light2.GetID());
    }
}
