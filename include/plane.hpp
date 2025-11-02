#ifndef PLANE_HPP
#define PLANE_HPP

#include "object.hpp"
#include "physics.hpp"
#include "raylib.h"
#include <ode/ode.h>

class Plane : public Object {
private:
    Vector2 size;
    Color color;
    dGeomID geom;
    PhysicsWorld* physics;

public:
    Plane(Vector3 position, Vector2 planeSize, Color planeColor, PhysicsWorld* physicsWorld);
    virtual ~Plane();

    // Override virtual functions
    void Draw(Camera3D camera) override;
    const char* GetType() const override;
};

#endif
