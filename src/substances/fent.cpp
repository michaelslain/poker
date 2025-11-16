#include "substances/fent.hpp"
#include "core/physics.hpp"
#include "entities/player.hpp"

Fent::Fent(Vector3 pos, PhysicsWorld* physics)
    : Substance(pos, (Color){50, 50, 50, 255}, physics)  // Dark gray/black color for fentanyl
{
}

Fent::~Fent() {
    // Cleanup handled by Substance base class
}

void Fent::Consume() {
    // Fentanyl triggers immediate death (overdose)
    Player* player = Player::GetGlobal();
    if (player) {
        player->TriggerDeath();
        TraceLog(LOG_INFO, "FENT: Overdose triggered - player will die");
    } else {
        TraceLog(LOG_WARNING, "FENT: Cannot trigger death - no global player instance");
    }
}

std::string Fent::GetType() const {
    return Substance::GetType() + "_fent";
}

Object* Fent::Clone(Vector3 newPos) const {
    PhysicsWorld* physics = PhysicsWorld::GetGlobal();
    return new Fent(newPos, physics);
}
