#ifndef SPAWNER_HPP
#define SPAWNER_HPP

#include "core/object.hpp"

class Spawner : public Object {
private:
    float radius;
    Object* templateObject;  // The object to spawn copies of
    int count;
    bool hasSpawned;
    
    void PerformSpawn();

public:
    // Constructor: spawns 'spawnCount' copies of 'obj' within 'spawnRadius'
    Spawner(Vector3 pos, float spawnRadius, Object* obj, int spawnCount);
    
    virtual ~Spawner();
    
    void Update(float deltaTime) override;
    std::string GetType() const override;
};

#endif
