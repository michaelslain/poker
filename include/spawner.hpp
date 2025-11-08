#ifndef SPAWNER_HPP
#define SPAWNER_HPP

#include "object.hpp"
#include "card.hpp"
#include "chip.hpp"
#include "physics.hpp"
#include "dom.hpp"

class Spawner : public Object {
private:
    float radius;

public:
    Spawner(Vector3 pos, float spawnRadius);
    virtual ~Spawner() = default;
    
    // Spawning methods
    void SpawnCards(Suit suit, Rank rank, int count, PhysicsWorld* physics, DOM* dom);
    void SpawnChips(int value, int count, PhysicsWorld* physics, DOM* dom);
    void SpawnPistols(int count, PhysicsWorld* physics, DOM* dom);
    
    std::string GetType() const override;
};

#endif
