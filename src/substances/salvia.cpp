#include "substances/salvia.hpp"
#include "core/level_manager.hpp"
#include "rendering/psychedelic_manager.hpp"

Salvia::Salvia(Vector3 pos)
    : Substance(pos, (Color){200, 255, 100, 255})  // Green color for salvia
{
}

Salvia::~Salvia() {
    // Cleanup handled by Substance base class
}

void Salvia::Consume() {
    // Salvia triggers alternate dimension transition
    // Option B: Enter alternate dimension (parallel universe)
    // Player will be in same level number but alternate dimension
    // When they beat that level (reach stairs), they do option C (random jump)
    
    TraceLog(LOG_INFO, "SALVIA: Consumed! Starting trip...");
    
    LevelManager* levelManager = LevelManager::GetInstance();
    levelManager->EnterAlternateDimension();
    
    // Start psychedelic trip with lower intensity (0.5) and Salvia timing
    PsychedelicManager::StartTrip(0.5f, TripType::SALVIA);
    TraceLog(LOG_INFO, "SALVIA: Trip started - type=%d, isTripping=%d", 
             (int)PsychedelicManager::GetTripType(), 
             PsychedelicManager::IsTripping());
    
    // TODO: Trigger level regeneration to create alternate dimension version
    // This will be handled in main.cpp when we detect dimension change
}

std::string Salvia::GetType() const {
    return Substance::GetType() + "_salvia";
}

Object* Salvia::Clone(Vector3 newPos) const {
    return new Salvia(newPos);
}
