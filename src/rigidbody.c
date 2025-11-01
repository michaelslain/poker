#include "rigidbody.h"
#include "raymath.h"
#include "player.h"  // For collision category defines

void RigidBody_InitBox(RigidBody* rb, PhysicsWorld* physics, Vector3 position, Vector3 size, float mass) {
    Object_Init(&rb->base, position);
    rb->physics = physics;
    
    // Create dynamic body
    rb->body = dBodyCreate(physics->world);
    dBodySetPosition(rb->body, position.x, position.y, position.z);
    
    // Set mass
    dMass m;
    dMassSetBoxTotal(&m, mass, size.x, size.y, size.z);
    dBodySetMass(rb->body, &m);
    
    // Create box geometry and attach to body
    rb->geom = dCreateBox(physics->space, size.x, size.y, size.z);
    dGeomSetBody(rb->geom, rb->body);
    
    // Set collision category for items (cards, chips)
    // Items don't collide with player, only with ground and each other
    dGeomSetCategoryBits(rb->geom, COLLISION_CATEGORY_ITEM);
    dGeomSetCollideBits(rb->geom, ~COLLISION_CATEGORY_PLAYER);  // Collide with everything except player
}

void RigidBody_InitSphere(RigidBody* rb, PhysicsWorld* physics, Vector3 position, float radius, float mass) {
    Object_Init(&rb->base, position);
    rb->physics = physics;
    
    // Create dynamic body
    rb->body = dBodyCreate(physics->world);
    dBodySetPosition(rb->body, position.x, position.y, position.z);
    
    // Set mass
    dMass m;
    dMassSetSphereTotal(&m, mass, radius);
    dBodySetMass(rb->body, &m);
    
    // Create sphere geometry and attach to body
    rb->geom = dCreateSphere(physics->space, radius);
    dGeomSetBody(rb->geom, rb->body);
    
    // Set collision category for items (cards, chips)
    // Items don't collide with player, only with ground and each other
    dGeomSetCategoryBits(rb->geom, COLLISION_CATEGORY_ITEM);
    dGeomSetCollideBits(rb->geom, ~COLLISION_CATEGORY_PLAYER);  // Collide with everything except player
}

void RigidBody_Update(RigidBody* rb) {
    // Get position from ODE body
    const dReal* pos = dBodyGetPosition(rb->body);
    rb->base.position.x = (float)pos[0];
    rb->base.position.y = (float)pos[1];
    rb->base.position.z = (float)pos[2];
    
    // Get rotation from ODE body
    const dReal* rot = dBodyGetRotation(rb->body);
    
    // Convert ODE rotation matrix (3x4) to euler angles for Object rotation
    // This is a simplified conversion - for full accuracy we'd want quaternions
    rb->base.rotation.x = atan2f((float)rot[9], (float)rot[10]);
    rb->base.rotation.y = atan2f(-(float)rot[8], sqrtf((float)rot[9] * (float)rot[9] + (float)rot[10] * (float)rot[10]));
    rb->base.rotation.z = atan2f((float)rot[4], (float)rot[0]);
}

Matrix RigidBody_GetRotationMatrix(RigidBody* rb) {
    const dReal* rot = dBodyGetRotation(rb->body);
    
    // Convert ODE rotation matrix (3x4 row-major) to raylib Matrix (4x4 column-major)
    Matrix m = {
        (float)rot[0], (float)rot[4], (float)rot[8],  0.0f,
        (float)rot[1], (float)rot[5], (float)rot[9],  0.0f,
        (float)rot[2], (float)rot[6], (float)rot[10], 0.0f,
        0.0f,          0.0f,          0.0f,           1.0f
    };
    
    return m;
}

void RigidBody_Cleanup(RigidBody* rb) {
    if (rb->geom) {
        dGeomDestroy(rb->geom);
        rb->geom = NULL;
    }
    if (rb->body) {
        dBodyDestroy(rb->body);
        rb->body = NULL;
    }
}
