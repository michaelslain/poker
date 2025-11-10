#pragma once

#include "object.hpp"
#include "physics.hpp"
#include <ode/ode.h>

// Collision shape types
enum CollisionShape {
    COLLISION_SHAPE_BOX,
    COLLISION_SHAPE_SPHERE,
    COLLISION_SHAPE_CAPSULE,
    COLLISION_SHAPE_PLANE
};

// Collider component - can be attached to any Object
// Handles both static (no body) and dynamic (with body) collision
class Collider {
public:
    Collider();
    ~Collider();
    
    // Static collision (walls, tables, etc.)
    void InitStatic(PhysicsWorld* physics, CollisionShape shape, Vector3 size, Vector3 offset = {0, 0, 0});
    
    // Dynamic collision (items, player, etc.)
    void InitDynamic(PhysicsWorld* physics, CollisionShape shape, Vector3 size, float mass, Vector3 offset = {0, 0, 0});
    
    // Update position/rotation from parent object or physics body
    void UpdateFromObject(Object* obj);
    void UpdateObjectFromPhysics(Object* obj);
    
    // Collision filtering
    void SetCollisionBits(unsigned long category, unsigned long collideMask);
    
    // Accessors
    dGeomID GetGeom() const { return geom; }
    dBodyID GetBody() const { return body; }
    bool IsDynamic() const { return body != nullptr; }
    
private:
    dGeomID geom;
    dBodyID body;
    PhysicsWorld* physics;
    Vector3 offset;  // Offset from parent object position
    CollisionShape shape;
    Vector3 size;    // Size/radius parameters
};
