#include "vodka.hpp"

Vodka::Vodka(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){220, 220, 240, 255}, physics)  // Clear/light blue color for vodka
{
}

Vodka::~Vodka() {
    // Cleanup handled by Substance base class
}

void Vodka::Consume() {
    // TODO: Implement vodka effects (drunk state, blurred vision, etc.)
    // For now, just placeholder
}

std::string Vodka::GetType() const {
    return Substance::GetType() + "_vodka";
}

Object* Vodka::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Vodka(newPos, physics);
}
