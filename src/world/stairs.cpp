#include "world/stairs.hpp"
#include <ode/ode.h>

// Collision category for stairs
#define COLLISION_CATEGORY_STAIRS (1 << 4)

Stairs::Stairs(Vector3 pos, Vector3 stairSize, Color stairColor, PhysicsWorld* physicsWorld)
    : Object(), size(stairSize), color(stairColor), physics(physicsWorld), transitionTriggered(false)
{
    position = pos;
    
    // Create static box geometry for stairs (no body - static)
    geom = dCreateBox(physics->space, size.x, size.y, size.z);
    dGeomSetPosition(geom, position.x, position.y, position.z);
    
    // Set collision category and mask
    dGeomSetCategoryBits(geom, COLLISION_CATEGORY_STAIRS);
    dGeomSetCollideBits(geom, (1 << 0));  // Only collide with player
}

Stairs::~Stairs() {
    if (geom) {
        dGeomDestroy(geom);
    }
}

void Stairs::Update(float deltaTime) {
    // Stairs are static - no update needed
    (void)deltaTime;  // Suppress unused parameter warning
}

void Stairs::Draw(Camera3D camera) {
    (void)camera;  // Suppress unused parameter warning
    
    // Draw stairs as a series of steps going upward
    int numSteps = 5;
    float stepHeight = size.y / numSteps;
    float stepDepth = size.z / numSteps;
    
    for (int i = 0; i < numSteps; i++) {
        Vector3 stepPos = {
            position.x,
            position.y - size.y / 2.0f + stepHeight * (i + 0.5f),
            position.z - size.z / 2.0f + stepDepth * (i + 0.5f)
        };
        
        Vector3 stepSize = {
            size.x,
            stepHeight,
            stepDepth
        };
        
        DrawCubeV(stepPos, stepSize, color);
        DrawCubeWiresV(stepPos, stepSize, BLACK);
    }
}

std::string Stairs::GetType() const {
    return Object::GetType() + "_stairs";
}

bool Stairs::CheckPlayerCollision(dGeomID playerGeom) {
    if (transitionTriggered) {
        return false;  // Already triggered
    }
    
    // Check collision with player
    dContactGeom contacts[4];
    int numContacts = dCollide(geom, playerGeom, 4, contacts, sizeof(dContactGeom));
    
    if (numContacts > 0) {
        transitionTriggered = true;
        return true;
    }
    
    return false;
}
