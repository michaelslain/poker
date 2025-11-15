#include "world/floor.hpp"
#include "rendering/lighting_manager.hpp"

Floor::Floor(Vector3 position, Vector2 floorSize, Color floorColor, PhysicsWorld* physicsWorld)
    : Object(position), size(floorSize), color(floorColor)
{
    // Initialize static plane collision (normal pointing up: Y+)
    if (physicsWorld) {
        // For plane: size = normal vector (0, 1, 0), offset.x = distance (0)
        collider.InitStatic(physicsWorld, COLLISION_SHAPE_PLANE, {0, 1, 0}, {0, 0, 0});
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
    DrawModel(model, position, 1.0f, color);
}

std::string Floor::GetType() const {
    return Object::GetType() + "_floor";
}
