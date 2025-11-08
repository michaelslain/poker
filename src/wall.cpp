#include "wall.hpp"
#include "light.hpp"

Wall::Wall(Vector3 position, Vector3 wallSize, PhysicsWorld* physicsWorld)
    : Object(position), size(wallSize), color({25, 30, 10, 255}), geom(nullptr), physics(physicsWorld)
{
    // Create static box geometry for the wall
    if (physics) {
        geom = dCreateBox(physics->space, size.x, size.y, size.z);
        dGeomSetPosition(geom, position.x, position.y, position.z);
    }
    
    // Create model with proper normals for lighting
    model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    model.materials[0].shader = LightSource::GetLightingShader();
}

Wall::~Wall() {
    if (geom) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    UnloadModel(model);
}

void Wall::Draw(Camera3D camera) {
    (void)camera;
    DrawModel(model, position, 1.0f, color);
    DrawCubeWiresV(position, size, BLACK);
}

std::string Wall::GetType() const {
    return "wall";
}
