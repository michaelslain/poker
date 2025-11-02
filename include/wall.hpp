#ifndef WALL_HPP
#define WALL_HPP

#include "object.hpp"
#include "physics.hpp"
#include "raylib.h"
#include <ode/ode.h>

class Wall : public Object {
private:
    Vector3 size;
    Color color;
    dGeomID geom;
    PhysicsWorld* physics;
    Model model;

public:
    Wall(Vector3 position, Vector3 wallSize, PhysicsWorld* physicsWorld);
    virtual ~Wall();

    // Override virtual functions
    void Draw(Camera3D camera) override;
    const char* GetType() const override;
    
    // Accessor for geometry
    dGeomID GetGeom() const { return geom; }
};

#endif
