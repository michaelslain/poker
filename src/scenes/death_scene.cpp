#include "scenes/death_scene.hpp"
#include "core/object.hpp"
#include "core/scene.hpp"
#include "core/physics.hpp"
#include <raylib.h>

// Simple object that just renders "THE END" text
class DeathSceneObject : public Object {
public:
    DeathSceneObject() : Object({0, 0, 0}) {}
    
    void Update(float deltaTime) override {
        (void)deltaTime; // Unused
    }
    
    void Draw(Camera3D camera) override {
        (void)camera; // Unused - we draw in 2D
        
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        const char* text = "THE END";
        int fontSize = 60;
        int textWidth = MeasureText(text, fontSize);

        DrawText(text,
                 screenWidth / 2 - textWidth / 2,
                 screenHeight / 2 - fontSize / 2,
                 fontSize,
                 WHITE);
    }
    
    std::string GetType() const override {
        return "object_death_scene_renderer";
    }
};

Scene* CreateDeathScene(PhysicsWorld* physics) {
    (void)physics; // Unused - death scene has no physics
    
    std::vector<Object*> objects;
    objects.push_back(new DeathSceneObject());
    
    return new Scene("death", objects);
}
