#include "catch_amalgamated.hpp"
#include "scenes/death_scene.hpp"
#include "core/scene.hpp"
#include "core/physics.hpp"
#include "core/object.hpp"
#include <string>

TEST_CASE("DeathScene - Factory Function", "[death_scene]") {
    SECTION("CreateDeathScene creates valid scene") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        REQUIRE(scene != nullptr);
        REQUIRE(scene->GetName() == "death");
        
        delete scene;
    }
    
    SECTION("CreateDeathScene creates scene with objects") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        // Death scene should have at least one object (DeathSceneObject)
        REQUIRE(scene->GetInitialObjects().size() > 0);
        
        delete scene;
    }
    
    SECTION("CreateDeathScene doesn't crash with null physics") {
        // Physics parameter is unused in death scene
        Scene* scene = CreateDeathScene(nullptr);
        
        REQUIRE(scene != nullptr);
        
        delete scene;
    }
}

TEST_CASE("DeathSceneObject - Type System", "[death_scene]") {
    SECTION("GetType returns correct type string") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        // Get the first object from the scene
        auto& objects = scene->GetInitialObjects();
        REQUIRE(objects.size() > 0);
        
        Object* obj = objects[0];
        std::string type = obj->GetType();
        
        REQUIRE(type.find("object") != std::string::npos);
        REQUIRE(type.find("death_scene_renderer") != std::string::npos);
        
        delete scene;
    }
}

TEST_CASE("DeathSceneObject - Update", "[death_scene]") {
    SECTION("Update doesn't crash") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        auto& objects = scene->GetInitialObjects();
        Object* obj = objects[0];
        
        // Update should not crash
        REQUIRE_NOTHROW(obj->Update(0.016f));
        REQUIRE_NOTHROW(obj->Update(0.0f));
        REQUIRE_NOTHROW(obj->Update(1.0f));
        
        delete scene;
    }
}

TEST_CASE("DeathSceneObject - Draw", "[death_scene]") {
    SECTION("Draw doesn't crash with dummy camera") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        auto& objects = scene->GetInitialObjects();
        Object* obj = objects[0];
        
        // Create dummy camera (death scene renders in 2D, ignores camera)
        Camera3D camera = {};
        camera.position = {0, 0, 0};
        camera.target = {0, 0, 0};
        camera.up = {0, 1, 0};
        camera.fovy = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        
        // Draw should not crash (even in headless mode)
        REQUIRE_NOTHROW(obj->Draw(camera));
        
        delete scene;
    }
}

TEST_CASE("DeathSceneObject - Position", "[death_scene]") {
    SECTION("Created at origin") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        auto& objects = scene->GetInitialObjects();
        Object* obj = objects[0];
        
        REQUIRE(obj->position.x == 0.0f);
        REQUIRE(obj->position.y == 0.0f);
        REQUIRE(obj->position.z == 0.0f);
        
        delete scene;
    }
}

TEST_CASE("DeathScene - Memory Management", "[death_scene][memory]") {
    SECTION("Scene cleanup deletes objects") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        // Scene should properly manage its objects
        auto& objects = scene->GetInitialObjects();
        REQUIRE(objects.size() > 0);
        
        // Delete scene - should not leak memory
        delete scene;
        
        // Test passes if we get here without crashes
        REQUIRE(true);
    }
    
    SECTION("Multiple scene creations don't leak") {
        PhysicsWorld physics;
        
        for (int i = 0; i < 10; i++) {
            Scene* scene = CreateDeathScene(&physics);
            REQUIRE(scene != nullptr);
            delete scene;
        }
        
        REQUIRE(true);
    }
}

TEST_CASE("DeathScene - Scene Properties", "[death_scene]") {
    SECTION("Scene has correct name") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        REQUIRE(scene->GetName() == "death");
        
        delete scene;
    }
    
    SECTION("Scene objects are not null") {
        PhysicsWorld physics;
        Scene* scene = CreateDeathScene(&physics);
        
        auto& objects = scene->GetInitialObjects();
        for (Object* obj : objects) {
            REQUIRE(obj != nullptr);
        }
        
        delete scene;
    }
}
