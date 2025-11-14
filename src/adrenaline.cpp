#include "adrenaline.hpp"

Adrenaline::Adrenaline(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, RED, physics)  // Red color for adrenaline
{
}

Adrenaline::~Adrenaline() {
    // Cleanup handled by Substance base class
}

void Adrenaline::Consume() {
    // TODO: Implement adrenaline effects (speed boost, etc.)
    // For now, just placeholder
}

std::string Adrenaline::GetType() const {
    return Substance::GetType() + "_adrenaline";
}

Object* Adrenaline::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Adrenaline(newPos, physics);
}
