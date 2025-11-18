#include "substances/cocaine.hpp"

Cocaine::Cocaine(Vector3 pos)
    : Substance(pos, WHITE)  // White color for cocaine
{
}

Cocaine::~Cocaine() {
    // Cleanup handled by Substance base class
}

void Cocaine::Consume() {
    // TODO: Implement cocaine effects (energy boost, etc.)
    // For now, just placeholder
}

std::string Cocaine::GetType() const {
    return Substance::GetType() + "_cocaine";
}

Object* Cocaine::Clone(Vector3 newPos) const {
    return new Cocaine(newPos);
}
