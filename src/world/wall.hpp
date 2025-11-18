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
    bool needsColliderUpdate;  // Flag to update collider after rotation is set
    
    Wall(Vector3 position, Vector3 wallSize);
    virtual ~Wall();
    
    // Override virtual functions
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Accessor for collider
    Collider* GetCollider() { return &collider; }
};
