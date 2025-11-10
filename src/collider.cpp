#include "collider.hpp"
#include <cmath>

Collider::Collider() 
    : geom(nullptr), body(nullptr), physics(nullptr), offset({0, 0, 0}), shape(COLLISION_SHAPE_BOX), size({1, 1, 1})
{
}

Collider::~Collider() {
    if (geom) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    if (body) {
        dBodyDestroy(body);
        body = nullptr;
    }
}

void Collider::InitStatic(PhysicsWorld* phys, CollisionShape shapeType, Vector3 shapeSize, Vector3 shapeOffset) {
    physics = phys;
    shape = shapeType;
    size = shapeSize;
    offset = shapeOffset;
    
    if (!physics) return;
    
    // Create geometry based on shape
    switch (shape) {
        case COLLISION_SHAPE_BOX:
            geom = dCreateBox(physics->space, size.x, size.y, size.z);
            break;
        case COLLISION_SHAPE_SPHERE:
            geom = dCreateSphere(physics->space, size.x); // size.x = radius
            break;
        case COLLISION_SHAPE_CAPSULE:
            geom = dCreateCapsule(physics->space, size.x, size.y); // size.x = radius, size.y = length
            break;
        case COLLISION_SHAPE_PLANE:
            // size.x = normal.x, size.y = normal.y, size.z = normal.z, offset.x = distance
            geom = dCreatePlane(physics->space, size.x, size.y, size.z, offset.x);
            break;
    }
    
    // Set default collision bits (collide with everything)
    if (geom) {
        dGeomSetCategoryBits(geom, ~0);
        dGeomSetCollideBits(geom, ~0);
    }
    
    // Note: body stays nullptr for static collision
}

void Collider::InitDynamic(PhysicsWorld* phys, CollisionShape shapeType, Vector3 shapeSize, float mass, Vector3 shapeOffset) {
    physics = phys;
    shape = shapeType;
    size = shapeSize;
    offset = shapeOffset;
    
    if (!physics) return;
    
    // Create dynamic body
    body = dBodyCreate(physics->world);
    
    // Set mass based on shape
    dMass m;
    switch (shape) {
        case COLLISION_SHAPE_BOX:
            dMassSetBoxTotal(&m, mass, size.x, size.y, size.z);
            geom = dCreateBox(physics->space, size.x, size.y, size.z);
            break;
        case COLLISION_SHAPE_SPHERE:
            dMassSetSphereTotal(&m, mass, size.x);
            geom = dCreateSphere(physics->space, size.x);
            break;
        case COLLISION_SHAPE_CAPSULE:
            dMassSetCapsuleTotal(&m, mass, 3, size.x, size.y); // 3 = Z-axis aligned
            geom = dCreateCapsule(physics->space, size.x, size.y);
            break;
        case COLLISION_SHAPE_PLANE:
            // Planes can't be dynamic
            return;
    }
    
    dBodySetMass(body, &m);
    
    // Attach geometry to body
    if (geom) {
        dGeomSetBody(geom, body);
        
        // Set default collision bits
        dGeomSetCategoryBits(geom, ~0);
        dGeomSetCollideBits(geom, ~0);
    }
}

void Collider::UpdateFromObject(Object* obj) {
    if (!geom || !obj) return;
    
    Vector3 finalPos = {
        obj->position.x + offset.x,
        obj->position.y + offset.y,
        obj->position.z + offset.z
    };
    
    if (body) {
        // Dynamic - set body position
        dBodySetPosition(body, finalPos.x, finalPos.y, finalPos.z);
    } else if (shape != COLLISION_SHAPE_PLANE) {
        // Static non-plane - set geom position directly
        dGeomSetPosition(geom, finalPos.x, finalPos.y, finalPos.z);
    }
    // Planes don't need position updates
}

void Collider::UpdateObjectFromPhysics(Object* obj) {
    if (!body || !obj) return;
    
    // Get position from physics body
    const dReal* pos = dBodyGetPosition(body);
    obj->position.x = (float)pos[0] - offset.x;
    obj->position.y = (float)pos[1] - offset.y;
    obj->position.z = (float)pos[2] - offset.z;
    
    // Get rotation from physics body
    const dReal* rot = dBodyGetRotation(body);
    obj->rotation.x = atan2f((float)rot[9], (float)rot[10]);
    obj->rotation.y = atan2f(-(float)rot[8], sqrtf((float)rot[9] * (float)rot[9] + (float)rot[10] * (float)rot[10]));
    obj->rotation.z = atan2f((float)rot[4], (float)rot[0]);
}

void Collider::SetCollisionBits(unsigned long category, unsigned long collideMask) {
    if (geom) {
        dGeomSetCategoryBits(geom, category);
        dGeomSetCollideBits(geom, collideMask);
    }
}
