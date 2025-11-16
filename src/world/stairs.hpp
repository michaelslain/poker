#ifndef STAIRS_HPP
#define STAIRS_HPP

#include "core/object.hpp"
#include "core/physics.hpp"
#include "raylib.h"

// Stairs object for transitioning between levels
// Player walks onto stairs to trigger level transition
// Not interactable - uses collision detection instead
class Stairs : public Object {
private:
    dGeomID geom;               // Collision geometry
    PhysicsWorld* physics;      // Reference to physics world
    Vector3 size;               // Size of stairs (width, height, depth)
    Color color;                // Visual color
    bool transitionTriggered;   // Prevent multiple triggers

public:
    // Constructor
    Stairs(Vector3 pos, Vector3 stairSize, Color stairColor, PhysicsWorld* physicsWorld);
    
    // Destructor
    ~Stairs();
    
    // Override base class methods
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Accessors
    dGeomID GetGeom() const { return geom; }
    bool IsTransitionTriggered() const { return transitionTriggered; }
    void ResetTransition() { transitionTriggered = false; }
    
    // Check collision with player
    bool CheckPlayerCollision(dGeomID playerGeom);
};

#endif // STAIRS_HPP
