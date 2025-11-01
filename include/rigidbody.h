#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "object.h"
#include "physics.h"
#include <ode/ode.h>

typedef struct {
    Object base;
    dBodyID body;
    dGeomID geom;
    PhysicsWorld* physics;  // Reference to physics world
} RigidBody;

// Initialize a box rigid body
void RigidBody_InitBox(RigidBody* rb, PhysicsWorld* physics, Vector3 position, Vector3 size, float mass);

// Initialize a sphere rigid body
void RigidBody_InitSphere(RigidBody* rb, PhysicsWorld* physics, Vector3 position, float radius, float mass);

// Sync the Object position with the physics body
void RigidBody_Update(RigidBody* rb);

// Cleanup rigid body
void RigidBody_Cleanup(RigidBody* rb);

// Get rotation matrix from ODE body
Matrix RigidBody_GetRotationMatrix(RigidBody* rb);

#endif // RIGIDBODY_H
