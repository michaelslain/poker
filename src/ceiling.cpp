#include "ceiling.hpp"
#include "light.hpp"

Ceiling::Ceiling(Vector3 position, Vector2 ceilingSize, Color ceilingColor, PhysicsWorld* physicsWorld)
    : Object(position), size(ceilingSize), color(ceilingColor), geom(nullptr), physics(physicsWorld)
{
    // Create static plane geometry (normal pointing down: Y-)
    if (physics) {
        geom = dCreatePlane(physics->space, 0, -1, 0, -position.y);
    }
    
    // Create model with proper normals for lighting
    model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 10, 10));
    model.materials[0].shader = LightSource::GetLightingShader();
}

Ceiling::~Ceiling() {
    if (geom) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    UnloadModel(model);
}

void Ceiling::Draw(Camera3D camera) {
    (void)camera;
    DrawModel(model, position, 1.0f, color);
}

std::string Ceiling::GetType() const {
    return Object::GetType() + "_ceiling";
}
