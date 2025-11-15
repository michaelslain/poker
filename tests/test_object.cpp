#include "catch_amalgamated.hpp"
#include "core/object.hpp"

TEST_CASE("Object - Construction", "[object]") {
    SECTION("Create object with default position") {
        Object obj;
        REQUIRE(obj.position.x == 0.0f);
        REQUIRE(obj.position.y == 0.0f);
        REQUIRE(obj.position.z == 0.0f);
    }
    
    SECTION("Create object with custom position") {
        Object obj({5.0f, 10.0f, 15.0f});
        REQUIRE(obj.position.x == 5.0f);
        REQUIRE(obj.position.y == 10.0f);
        REQUIRE(obj.position.z == 15.0f);
    }
}

TEST_CASE("Object - Position", "[object]") {
    Object obj;
    
    SECTION("Can modify position") {
        obj.position = {1.0f, 2.0f, 3.0f};
        REQUIRE(obj.position.x == 1.0f);
        REQUIRE(obj.position.y == 2.0f);
        REQUIRE(obj.position.z == 3.0f);
    }
}

TEST_CASE("Object - Rotation", "[object]") {
    Object obj;
    
    SECTION("Default rotation is zero") {
        REQUIRE(obj.rotation.x == 0.0f);
        REQUIRE(obj.rotation.y == 0.0f);
        REQUIRE(obj.rotation.z == 0.0f);
    }
    
    SECTION("Can set rotation") {
        obj.rotation = {90.0f, 45.0f, 180.0f};
        REQUIRE(obj.rotation.x == 90.0f);
        REQUIRE(obj.rotation.y == 45.0f);
        REQUIRE(obj.rotation.z == 180.0f);
    }
}

TEST_CASE("Object - Scale", "[object]") {
    Object obj;
    
    SECTION("Default scale is 1.0") {
        REQUIRE(obj.scale.x == 1.0f);
        REQUIRE(obj.scale.y == 1.0f);
        REQUIRE(obj.scale.z == 1.0f);
    }
    
    SECTION("Can set scale") {
        obj.scale = {2.0f, 0.5f, 1.5f};
        REQUIRE(obj.scale.x == 2.0f);
        REQUIRE(obj.scale.y == 0.5f);
        REQUIRE(obj.scale.z == 1.5f);
    }
}

TEST_CASE("Object - GetType", "[object]") {
    Object obj;
    REQUIRE(obj.GetType() == "object");
}

TEST_CASE("Object - ID", "[object]") {
    Object obj1;
    Object obj2;
    
    SECTION("Each object has unique ID") {
        REQUIRE(obj1.GetID() != obj2.GetID());
    }
}
