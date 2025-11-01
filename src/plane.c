#include "plane.h"

void Plane_Init(Plane* plane, Vector3 position, Vector2 size, Color color, PhysicsWorld* physics) {
    Object_Init(&plane->base, position);
    plane->size = size;
    plane->color = color;
    plane->physics = physics;
    
    // Create static plane geometry (normal pointing up: Y+)
    plane->geom = dCreatePlane(physics->space, 0, 1, 0, 0);
}

void Plane_Draw(Plane* plane) {
    DrawPlane(plane->base.position, plane->size, plane->color);
}

void Plane_Cleanup(Plane* plane) {
    if (plane->geom) {
        dGeomDestroy(plane->geom);
        plane->geom = NULL;
    }
}
