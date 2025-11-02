#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <ode/ode.h>
#include "raylib.h"

class PhysicsWorld {
public:
    dWorldID world;
    dSpaceID space;
    dJointGroupID contactGroup;

    PhysicsWorld();
    ~PhysicsWorld();

    void Step(float deltaTime);
    static void NearCallback(void* data, dGeomID o1, dGeomID o2);
};

#endif
