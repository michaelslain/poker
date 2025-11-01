#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include "raylib.h"

// Physics world wrapper
typedef struct {
    dWorldID world;
    dSpaceID space;
    dJointGroupID contactGroup;
} PhysicsWorld;

// Initialize the physics world
void Physics_Init(PhysicsWorld* physics);

// Step the physics simulation
void Physics_Step(PhysicsWorld* physics, float deltaTime);

// Cleanup physics world
void Physics_Cleanup(PhysicsWorld* physics);

// Collision callback
void Physics_NearCallback(void* data, dGeomID o1, dGeomID o2);

#endif // PHYSICS_H
