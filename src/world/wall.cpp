#include "world/wall.hpp"
#include "rendering/lighting_manager.hpp"
#include "rlgl.h"
#include "core/collision_categories.hpp"

Wall::Wall(Vector3 position, Vector3 wallSize)
    : Object(position), size(wallSize), color({25, 30, 10, 255}), needsColliderUpdate(false)
{
    // For 2D walls: size.x = width, size.y = height, size.z should always be 0
    // Use a thin box for collision (thickness = 0.2 units)
    const float COLLISION_THICKNESS = 0.2f;
    Vector3 physicsSize = {size.x, size.y, COLLISION_THICKNESS};
    
    TraceLog(LOG_INFO, "WALL: Creating wall at (%.1f, %.1f, %.1f) size (%.1f x %.1f) rotation.y=%.1f",
             position.x, position.y, position.z, size.x, size.y, rotation.y);
    
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    if (physics) {
        collider.InitStatic(physics, COLLISION_SHAPE_BOX, physicsSize);
        
        // Set collision bits: WORLD category
        collider.SetCollisionBits(COLLISION_CATEGORY_WORLD, COLLISION_MASK_WORLD);
        
        // Set Object data for collision detection
        if (collider.GetGeom()) {
            dGeomSetData(collider.GetGeom(), this);
        }
        
        // Update immediately for walls without rotation
        collider.UpdateFromObject(this);
        needsColliderUpdate = false;
    }
    
    // Create model as a flat plane (using GenMeshPlane which is 2D)
    // GenMeshPlane creates XZ plane, so we rotate it to XY for vertical walls
    Mesh planeMesh = GenMeshPlane(size.x, size.y, 1, 1);
    model = LoadModelFromMesh(planeMesh);
    model.materials[0].shader = LightingManager::GetLightingShader();
}

void Wall::Update(float deltaTime) {
    (void)deltaTime;
    
    // Update collider on first frame if rotation was set after construction
    if (needsColliderUpdate) {
        TraceLog(LOG_INFO, "WALL: Updating collider at (%.1f, %.1f, %.1f) with rotation.y=%.1f",
                 position.x, position.y, position.z, rotation.y);
        collider.UpdateFromObject(this);
        needsColliderUpdate = false;
    }
}

Wall::~Wall() {
    UnloadModel(model);
}

void Wall::Draw(Camera3D camera) {
    (void)camera;
    
    // Draw the plane twice to make it visible from both sides
    // GenMeshPlane creates horizontal plane, rotate 90 degrees around X to make it vertical
    
    // Draw front side
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(rotation.y, 0, 1, 0);  // Apply Y rotation for hallway walls
        rlRotatef(90, 1, 0, 0);  // Rotate to make it vertical (XY plane)
        DrawModel(model, {0, 0, 0}, 1.0f, color);
    rlPopMatrix();
    
    // Draw back side (flip by rotating -90 instead of +90 around X)
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(rotation.y, 0, 1, 0);  // Apply Y rotation for hallway walls
        rlRotatef(-90, 1, 0, 0);  // Rotate opposite direction for back side
        DrawModel(model, {0, 0, 0}, 1.0f, color);
    rlPopMatrix();
}

std::string Wall::GetType() const {
    return Object::GetType() + "_wall";
}
