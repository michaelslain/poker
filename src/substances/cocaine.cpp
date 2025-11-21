#include "substances/cocaine.hpp"
#include "entities/player.hpp"

Cocaine::Cocaine(Vector3 pos)
    : Substance(pos, WHITE)  // White color for cocaine
{
}

Cocaine::~Cocaine() {
    // Cleanup handled by Substance base class
}

void Cocaine::Consume() {
    Player* player = Player::GetGlobal();
    if (player) {
        // Double speed for 30 seconds
        player->ApplySpeedBoost(10.0f, 30.0f);
        TraceLog(LOG_INFO, "COCAINE: Consumed - speed boost activated");
    }
}

std::string Cocaine::GetType() const {
    return Substance::GetType() + "_cocaine";
}

Object* Cocaine::Clone(Vector3 newPos) const {
    return new Cocaine(newPos);
}
