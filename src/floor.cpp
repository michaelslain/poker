#include "floor.hpp"
#include "light.hpp"

Floor::Floor(Vector3 position, Vector2 floorSize, Color floorColor, PhysicsWorld* physicsWorld)
    : Object(position), size(floorSize), color(floorColor), geom(nullptr), physics(physicsWorld)
{
    // Create static plane geometry (normal pointing up: Y+)
    if (physics) {
        geom = dCreatePlane(physics->space, 0, 1, 0, 0);
    }
    
    // Create model with proper normals for lighting
    model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 10, 10));
    model.materials[0].shader = LightSource::GetLightingShader();
}

Floor::~Floor() {
    if (geom) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    UnloadModel(model);
}

void Floor::Draw(Camera3D camera) {
    (void)camera;
    DrawModel(model, position, 1.0f, color);
}

std::string Floor::GetType() const {
    return "floor";
}
