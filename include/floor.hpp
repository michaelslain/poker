#ifndef FLOOR_HPP
#define FLOOR_HPP

#include "object.hpp"
#include "physics.hpp"
#include "raylib.h"
#include <ode/ode.h>

class Floor : public Object {
private:
    Vector2 size;
    Color color;
    dGeomID geom;
    PhysicsWorld* physics;
    Model model;

public:
    Floor(Vector3 position, Vector2 floorSize, Color floorColor, PhysicsWorld* physicsWorld);
    virtual ~Floor();

    // Override virtual functions
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
};

#endif
