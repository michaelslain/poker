#include "weed.hpp"

Weed::Weed(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){50, 150, 50, 255}, physics)  // Dark green color for weed
{
}

Weed::~Weed() {
    // Cleanup handled by Substance base class
}

void Weed::Consume() {
    // TODO: Implement weed effects (relaxation, slow movement, etc.)
    // For now, just placeholder
}

std::string Weed::GetType() const {
    return Substance::GetType() + "_weed";
}

Object* Weed::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Weed(newPos, physics);
}
