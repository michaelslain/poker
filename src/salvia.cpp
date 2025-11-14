#include "salvia.hpp"

Salvia::Salvia(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){100, 200, 100, 255}, physics)  // Green color for salvia
{
}

Salvia::~Salvia() {
    // Cleanup handled by Substance base class
}

void Salvia::Consume() {
    // TODO: Implement salvia effects (hallucinations, etc.)
    // For now, just placeholder
}

std::string Salvia::GetType() const {
    return Substance::GetType() + "_salvia";
}

Object* Salvia::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Salvia(newPos, physics);
}
