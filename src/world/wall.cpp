#include "world/wall.hpp"
#include "rendering/lighting_manager.hpp"

Wall::Wall(Vector3 position, Vector3 wallSize, PhysicsWorld* physicsWorld)
    : Object(position), size(wallSize), color({25, 30, 10, 255})
{
    // Initialize static box collision
    if (physicsWorld) {
        collider.InitStatic(physicsWorld, COLLISION_SHAPE_BOX, size);
        collider.UpdateFromObject(this);
    }
    
    // Create model with proper normals for lighting
    model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    model.materials[0].shader = LightingManager::GetLightingShader();
}

Wall::~Wall() {
    UnloadModel(model);
}

void Wall::Draw(Camera3D camera) {
    (void)camera;
    DrawModel(model, position, 1.0f, color);
    DrawCubeWiresV(position, size, BLACK);
}

std::string Wall::GetType() const {
    return Object::GetType() + "_wall";
}
