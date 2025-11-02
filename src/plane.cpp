#include "plane.hpp"

Plane::Plane(Vector3 position, Vector2 planeSize, Color planeColor, PhysicsWorld* physicsWorld)
    : Object(position), size(planeSize), color(planeColor), physics(physicsWorld), geom(nullptr)
{
    // Create static plane geometry (normal pointing up: Y+)
    geom = dCreatePlane(physics->space, 0, 1, 0, 0);
}

Plane::~Plane() {
    if (geom) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
}

void Plane::Draw(Camera3D camera) {
    (void)camera;
    DrawPlane(position, size, color);
}

const char* Plane::GetType() const {
    return "plane";
}
