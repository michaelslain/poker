#include "shrooms.hpp"

Shrooms::Shrooms(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){150, 100, 200, 255}, physics)  // Purple color for shrooms
{
}

Shrooms::~Shrooms() {
    // Cleanup handled by Substance base class
}

void Shrooms::Consume() {
    // TODO: Implement shrooms effects (visual distortions, etc.)
    // For now, just placeholder
}

std::string Shrooms::GetType() const {
    return Substance::GetType() + "_shrooms";
}

Object* Shrooms::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Shrooms(newPos, physics);
}
