#ifndef HOSPITAL_SCENE_HPP
#define HOSPITAL_SCENE_HPP

#include "core/physics.hpp"
#include "core/dom.hpp"
#include "raylib.h"

// Hospital starting scene (level 0)
// Currently empty room for spawning
// TODO: Add cutscene/narrative intro here eventually
class HospitalScene {
private:
    PhysicsWorld* physics;
    DOM* dom;
    
    static constexpr float ROOM_WIDTH = 15.0f;
    static constexpr float ROOM_DEPTH = 15.0f;
    static constexpr float FLOOR_HEIGHT = 0.0f;
    static constexpr float CEILING_HEIGHT = 5.0f;
    static constexpr float WALL_THICKNESS = 0.5f;

public:
    // Constructor
    HospitalScene(PhysicsWorld* physicsWorld, DOM* domInstance);
    
    // Generate hospital scene
    void Generate();
    
    // Get player spawn position
    Vector3 GetPlayerSpawnPosition() const;
    
    // Cleanup
    void Clear();
};

#endif // HOSPITAL_SCENE_HPP
