#include "substances/weed.hpp"

Weed::Weed(Vector3 pos)
    : Substance(pos, (Color){50, 150, 50, 255})  // Dark green color for weed
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
    return new Weed(newPos);
}
