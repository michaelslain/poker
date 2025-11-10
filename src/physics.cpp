#include "physics.hpp"

// Initialize static member
PhysicsWorld* PhysicsWorld::globalInstance = nullptr;

PhysicsWorld::PhysicsWorld() {
    // Initialize ODE
    dInitODE();
    
    // Create physics world
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactGroup = dJointGroupCreate(0);
    
    // Set gravity (negative Y is down)
    dWorldSetGravity(world, 0, -9.81, 0);
    
    // Set some default parameters
    dWorldSetCFM(world, 1e-5);
    dWorldSetERP(world, 0.2);
    dWorldSetContactMaxCorrectingVel(world, 0.9);
    dWorldSetContactSurfaceLayer(world, 0.001);
}

PhysicsWorld::~PhysicsWorld() {
    dJointGroupDestroy(contactGroup);
    dSpaceDestroy(space);
    dWorldDestroy(world);
    dCloseODE();
}

void PhysicsWorld::NearCallback(void* data, dGeomID o1, dGeomID o2) {
    PhysicsWorld* physics = static_cast<PhysicsWorld*>(data);
    
    // Get the bodies
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    
    // Exit without doing anything if the two bodies are connected by a joint
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;
    
    // Create contact joints for collision
    dContact contact[4];
    int n = dCollide(o1, o2, 4, &contact[0].geom, sizeof(dContact));
    
    for (int i = 0; i < n; i++) {
        // Set contact properties
        contact[i].surface.mode = dContactBounce | dContactSoftCFM;
        contact[i].surface.mu = 0.5;
        contact[i].surface.bounce = 0.3;
        contact[i].surface.bounce_vel = 0.1;
        contact[i].surface.soft_cfm = 0.01;
        
        // Create contact joint
        dJointID c = dJointCreateContact(physics->world, physics->contactGroup, &contact[i]);
        dJointAttach(c, b1, b2);
    }
}

void PhysicsWorld::Step(float deltaTime) {
    // ODE requires deltaTime > 0
    if (deltaTime <= 0.0f) return;
    
    // Cap deltaTime to prevent instability
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    
    // Check for collisions
    dSpaceCollide(space, this, &NearCallback);
    
    // Step the world
    dWorldQuickStep(world, deltaTime);
    
    // Remove all contact joints
    dJointGroupEmpty(contactGroup);
}

void PhysicsWorld::SetGlobal(PhysicsWorld* physics) {
    globalInstance = physics;
}

PhysicsWorld* PhysicsWorld::GetGlobal() {
    return globalInstance;
}
