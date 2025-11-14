#include "molly.hpp"

Molly::Molly(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){255, 150, 200, 255}, physics)  // Pink color for molly
{
}

Molly::~Molly() {
    // Cleanup handled by Substance base class
}

void Molly::Consume() {
    // TODO: Implement molly effects (euphoria, energy, etc.)
    // For now, just placeholder
}

std::string Molly::GetType() const {
    return Substance::GetType() + "_molly";
}

Object* Molly::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Molly(newPos, physics);
}
