#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <ode/ode.h>

class PhysicsWorld {
private:
    static PhysicsWorld* globalInstance;

public:
    dWorldID world;
    dSpaceID space;
    dJointGroupID contactGroup;

    PhysicsWorld();
    ~PhysicsWorld();

    void Step(float deltaTime);
    static void NearCallback(void* data, dGeomID o1, dGeomID o2);

    // Global instance management (similar to DOM)
    static void SetGlobal(PhysicsWorld* physics);
    static PhysicsWorld* GetGlobal();
};

#endif
