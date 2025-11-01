#ifndef PLANE_H
#define PLANE_H

#include "object.h"
#include "physics.h"
#include "raylib.h"
#include <ode/ode.h>

typedef struct {
    Object base;
    Vector2 size;
    Color color;
    dGeomID geom;
    PhysicsWorld* physics;
} Plane;

void Plane_Init(Plane* plane, Vector3 position, Vector2 size, Color color, PhysicsWorld* physics);
void Plane_Draw(Plane* plane);
void Plane_Cleanup(Plane* plane);

const char* Plane_GetType(Object* obj);

#endif // PLANE_H
