#include "world/floor.hpp"
#include "core/collision_categories.hpp"
#include "rendering/lighting_manager.hpp"

Floor::Floor(Vector3 position, Vector2 floorSize, Color floorColor)
    : Object(position), size(floorSize), color(floorColor)
{
    // Get physics from global instance
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    
    // Initialize static plane collision (normal pointing up: Y+)
    if (physics) {
        // For plane: size = normal vector (0, 1, 0), offset.x = distance (Y position)
        // ODE plane equation: normal.x*x + normal.y*y + normal.z*z = distance
        // For Y-up plane at height h: 0*x + 1*y + 0*z = h
        collider.InitStatic(physics, COLLISION_SHAPE_PLANE, {0, 1, 0}, {position.y, 0, 0});
        
        // Set collision bits: WORLD category, collides with everything except other WORLD
        collider.SetCollisionBits(COLLISION_CATEGORY_WORLD, COLLISION_MASK_WORLD);
        
        // Set Object data for collision detection
        if (collider.GetGeom()) {
            dGeomSetData(collider.GetGeom(), this);
        }
        
        TraceLog(LOG_INFO, "FLOOR: Created floor plane at y=%.2f with normal (0,1,0)", position.y);
    }
    
    // Create model with proper normals for lighting
    model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 10, 10));
    model.materials[0].shader = LightingManager::GetLightingShader();
}

Floor::~Floor() {
    UnloadModel(model);
}

void Floor::Draw(Camera3D camera) {
    (void)camera;
    // Make floor semi-transparent for debugging (50% alpha)
    Color debugColor = color;
    debugColor.a = 128;  // 50% transparency
    DrawModel(model, position, 1.0f, debugColor);
}

std::string Floor::GetType() const {
    return Object::GetType() + "_floor";
}
