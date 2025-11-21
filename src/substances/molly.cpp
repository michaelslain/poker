#include "substances/molly.hpp"
#include "rendering/psychedelic_manager.hpp"

Molly::Molly(Vector3 pos)
    : Substance(pos, (Color){255, 150, 200, 255})  // Pink color for molly
{
}

Molly::~Molly() {
    // Cleanup handled by Substance base class
}

void Molly::Consume() {
    // Start molly trip: very low intensity with pink hue for 10 minutes
    PsychedelicManager::StartTrip(0.2f, TripType::MOLLY);
    TraceLog(LOG_INFO, "MOLLY: Consumed - pink psychedelic trip started (10 minutes, very low intensity)");
}

std::string Molly::GetType() const {
    return Substance::GetType() + "_molly";
}

Object* Molly::Clone(Vector3 newPos) const {
    return new Molly(newPos);
}
