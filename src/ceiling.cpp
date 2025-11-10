#include "ceiling.hpp"
#include "light.hpp"

Ceiling::Ceiling(Vector3 position, Vector2 ceilingSize, Color ceilingColor, PhysicsWorld* physicsWorld)
    : Object(position), size(ceilingSize), color(ceilingColor)
{
    // Initialize static plane collision (normal pointing down: Y-)
    if (physicsWorld) {
        // For plane: size = normal vector (0, -1, 0), offset.x = distance (-position.y)
        collider.InitStatic(physicsWorld, COLLISION_SHAPE_PLANE, {0, -1, 0}, {-position.y, 0, 0});
    }
    
    // Create model with proper normals for lighting
    model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 10, 10));
    model.materials[0].shader = LightSource::GetLightingShader();
}

Ceiling::~Ceiling() {
    UnloadModel(model);
}

void Ceiling::Draw(Camera3D camera) {
    (void)camera;
    DrawModel(model, position, 1.0f, color);
}

std::string Ceiling::GetType() const {
    return Object::GetType() + "_ceiling";
}
