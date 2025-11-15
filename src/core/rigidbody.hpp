#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include "core/object.hpp"
#include "core/physics.hpp"
#include <ode/ode.h>

class RigidBody : public Object {
public:
    dBodyID body;
    dGeomID geom;
    PhysicsWorld* physics;

    RigidBody(Vector3 position = {0.0f, 0.0f, 0.0f});
    virtual ~RigidBody();

    void InitBox(PhysicsWorld* physicsWorld, Vector3 pos, Vector3 size, float mass);
    void InitSphere(PhysicsWorld* physicsWorld, Vector3 pos, float radius, float mass);
    void Update(float deltaTime) override;
    Matrix GetRotationMatrix();
};

#endif
