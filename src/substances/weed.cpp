#include "substances/weed.hpp"
#include "entities/player.hpp"

Weed::Weed(Vector3 pos)
    : Substance(pos, (Color){50, 150, 50, 255})  // Dark green color for weed
{
}

Weed::~Weed() {
    // Cleanup handled by Substance base class
}

void Weed::Consume() {
    // Weed reduces insanity and resets min insanity (calming/relaxation effect)
    Player* player = Player::GetGlobal();
    if (player) {
        player->insanityManager.ReduceInsanity(0.5f);  // Reduce by 50%
        player->insanityManager.ResetMinInsanity();    // Reset kill-based insanity floor
        TraceLog(LOG_INFO, "WEED: Consumed - reduced insanity and reset min insanity");
    }
}

std::string Weed::GetType() const {
    return Substance::GetType() + "_weed";
}

Object* Weed::Clone(Vector3 newPos) const {
    return new Weed(newPos);
}
