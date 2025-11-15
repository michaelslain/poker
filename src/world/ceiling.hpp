#pragma once

#include "core/object.hpp"
#include "core/physics.hpp"
#include "core/collider.hpp"
#include "raylib.h"

class Ceiling : public Object {
private:
    Vector2 size;
    Color color;
    Collider collider;
    Model model;

public:
    Ceiling(Vector3 position, Vector2 ceilingSize, Color ceilingColor, PhysicsWorld* physicsWorld);
    virtual ~Ceiling();
    
    // Override virtual functions
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Accessor for collider
    Collider* GetCollider() { return &collider; }
};
