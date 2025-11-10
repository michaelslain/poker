#pragma once

#include "object.hpp"
#include "physics.hpp"
#include "collider.hpp"
#include "raylib.h"

class Floor : public Object {
private:
    Vector2 size;
    Color color;
    Collider collider;
    Model model;

public:
    Floor(Vector3 position, Vector2 floorSize, Color floorColor, PhysicsWorld* physicsWorld);
    virtual ~Floor();
    
    // Override virtual functions
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Accessor for collider
    Collider* GetCollider() { return &collider; }
};
