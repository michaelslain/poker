#include "substances/molly.hpp"

Molly::Molly(Vector3 pos)
    : Substance(pos, (Color){255, 150, 200, 255})  // Pink color for molly
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
    return new Molly(newPos);
}
