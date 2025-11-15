#include "core/rigidbody.hpp"
#include "entities/player.hpp"
#include <cmath>

RigidBody::RigidBody(Vector3 pos)
    : Object(pos), body(nullptr), geom(nullptr), physics(nullptr)
{
}

RigidBody::~RigidBody() {
    if (geom) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    if (body) {
        dBodyDestroy(body);
        body = nullptr;
    }
}

void RigidBody::InitBox(PhysicsWorld* physicsWorld, Vector3 pos, Vector3 size, float mass) {
    physics = physicsWorld;
    position = pos;
    
    // Create dynamic body
    body = dBodyCreate(physics->world);
    dBodySetPosition(body, pos.x, pos.y, pos.z);
    
    // Set mass
    dMass m;
    dMassSetBoxTotal(&m, mass, size.x, size.y, size.z);
    dBodySetMass(body, &m);
    
    // Create box geometry and attach to body
    geom = dCreateBox(physics->space, size.x, size.y, size.z);
    dGeomSetBody(geom, body);
    
    // Set collision category for items
    dGeomSetCategoryBits(geom, COLLISION_CATEGORY_ITEM);
    dGeomSetCollideBits(geom, ~COLLISION_CATEGORY_PLAYER);
}

void RigidBody::InitSphere(PhysicsWorld* physicsWorld, Vector3 pos, float radius, float mass) {
    physics = physicsWorld;
    position = pos;
    
    // Create dynamic body
    body = dBodyCreate(physics->world);
    dBodySetPosition(body, pos.x, pos.y, pos.z);
    
    // Set mass
    dMass m;
    dMassSetSphereTotal(&m, mass, radius);
    dBodySetMass(body, &m);
    
    // Create sphere geometry and attach to body
    geom = dCreateSphere(physics->space, radius);
    dGeomSetBody(geom, body);
    
    // Set collision category for items
    dGeomSetCategoryBits(geom, COLLISION_CATEGORY_ITEM);
    dGeomSetCollideBits(geom, ~COLLISION_CATEGORY_PLAYER);
}

void RigidBody::Update(float deltaTime) {
    (void)deltaTime;
    
    // Get position from ODE body
    const dReal* pos = dBodyGetPosition(body);
    position.x = (float)pos[0];
    position.y = (float)pos[1];
    position.z = (float)pos[2];
    
    // Get rotation from ODE body
    const dReal* rot = dBodyGetRotation(body);
    
    // Convert ODE rotation matrix to euler angles
    rotation.x = atan2f((float)rot[9], (float)rot[10]);
    rotation.y = atan2f(-(float)rot[8], sqrtf((float)rot[9] * (float)rot[9] + (float)rot[10] * (float)rot[10]));
    rotation.z = atan2f((float)rot[4], (float)rot[0]);
}

Matrix RigidBody::GetRotationMatrix() {
    const dReal* rot = dBodyGetRotation(body);
    
    // Convert ODE rotation matrix (3x4 row-major) to raylib Matrix (4x4 column-major)
    Matrix m = {
        (float)rot[0], (float)rot[4], (float)rot[8],  0.0f,
        (float)rot[1], (float)rot[5], (float)rot[9],  0.0f,
        (float)rot[2], (float)rot[6], (float)rot[10], 0.0f,
        0.0f,          0.0f,          0.0f,           1.0f
    };
    
    return m;
}
