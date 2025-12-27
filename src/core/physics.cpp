#include "core/physics.hpp"
#include "core/object.hpp"
#include "raylib.h"

// Initialize static member
PhysicsWorld* PhysicsWorld::globalInstance = nullptr;

PhysicsWorld::PhysicsWorld() {
    // Initialize ODE
    dInitODE();
    
    // Create physics world
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactGroup = dJointGroupCreate(0);
    
    // Set gravity (negative Y is down) - increased for more responsive feel
    dWorldSetGravity(world, 0, -25.0, 0);
    
    // Set physics parameters for stable ground collision (based on working ODE examples)
    dWorldSetCFM(world, 1e-5);  // Small global CFM (contacts use soft_cfm instead)
    dWorldSetERP(world, 0.96);  // High ERP for strong correction (0.96 from working example)
    dWorldSetContactMaxCorrectingVel(world, 10.0);  // Higher velocity for proper correction
    dWorldSetContactSurfaceLayer(world, 0.001);  // Small surface layer
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
    dContact contact[8];  // Increased from 4 to 8 for better stability
    int n = dCollide(o1, o2, 8, &contact[0].geom, sizeof(dContact));
    
    for (int i = 0; i < n; i++) {
        // Set contact properties based on working ODE example
        // Higher ERP (0.96) provides strong correction force
        // Higher CFM (2.0) makes contacts soft and stable
        contact[i].surface.mode = dContactSoftCFM | dContactSoftERP | dContactApprox1;
        contact[i].surface.mu = dInfinity;  // Infinite friction to prevent sliding
        contact[i].surface.soft_cfm = 0.001;  // VERY LOW CFM for hard contacts  
        contact[i].surface.soft_erp = 0.8;   // High ERP for strong correction
        
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
    
    // Use fixed timestep substepping to prevent tunneling
    // Physics runs at 60 Hz (substep = 1/60 = 0.01667 seconds)
    // Reduced from 120 Hz for performance optimization (50% physics cost reduction)
    const float fixedTimestep = 1.0f / 60.0f;
    int substeps = (int)(deltaTime / fixedTimestep) + 1;
    float actualSubstep = deltaTime / substeps;
    
    for (int i = 0; i < substeps; i++) {
        // Check for collisions
        dSpaceCollide(space, this, &NearCallback);
        
        // Step the world with small timestep
        dWorldQuickStep(world, actualSubstep);
        
        // Remove all contact joints
        dJointGroupEmpty(contactGroup);
    }
}

void PhysicsWorld::SetGlobal(PhysicsWorld* physics) {
    globalInstance = physics;
}

PhysicsWorld* PhysicsWorld::GetGlobal() {
    return globalInstance;
}
