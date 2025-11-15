#include "substances/shrooms.hpp"
#include "rendering/psychedelic_manager.hpp"

Shrooms::Shrooms(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){150, 100, 200, 255}, physics)  // Purple color for shrooms
{
}

Shrooms::~Shrooms() {
    // Cleanup handled by Substance base class
}

void Shrooms::Consume() {
    // Start psychedelic trip with full intensity
    PsychedelicManager::StartTrip(1.0f);
}

std::string Shrooms::GetType() const {
    return Substance::GetType() + "_shrooms";
}

Object* Shrooms::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Shrooms(newPos, physics);
}
