#pragma once

#include "core/object.hpp"
#include "core/physics.hpp"
#include "core/collider.hpp"
#include "raylib.h"

class Wall : public Object {
private:
    Vector3 size;
    Color color;
    Collider collider;
    Model model;

public:
    Wall(Vector3 position, Vector3 wallSize, PhysicsWorld* physicsWorld);
    virtual ~Wall();
    
    // Override virtual functions
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Accessor for collider
    Collider* GetCollider() { return &collider; }
};
