#ifndef LEVEL_GENERATOR_HPP
#define LEVEL_GENERATOR_HPP

#include "core/object.hpp"
#include "core/physics.hpp"
#include "core/dom.hpp"
#include "core/level_manager.hpp"
#include "raylib.h"
#include <vector>

// Room structure for level generation
struct Room {
    Vector2 position;  // Center position (x, z)
    Vector2 size;      // Width and depth
    bool hasPokerTable;
    bool isStartRoom;
    bool hasStairs;
    int gridX;         // Grid position X
    int gridZ;         // Grid position Z
    
    // Which sides connect to other rooms (doorways)
    bool connectsNorth;
    bool connectsSouth;
    bool connectsEast;
    bool connectsWest;
};

// Level generation class
// Uses algorithmic procedural generation to create casino-themed levels
class LevelGenerator {
private:
    PhysicsWorld* physics;
    DOM* dom;
    int levelNumber;
    
    std::vector<Room> rooms;
    
    // Generation parameters
    static constexpr int MIN_ROOMS = 3;
    static constexpr int MAX_ROOMS = 8;
    static constexpr float ROOM_MIN_SIZE = 8.0f;
    static constexpr float ROOM_MAX_SIZE = 15.0f;
    static constexpr float HALLWAY_WIDTH = 3.0f;
    static constexpr float FLOOR_HEIGHT = 0.0f;
    static constexpr float CEILING_HEIGHT = 5.0f;
    
    // Private generation methods
    void GenerateRooms(int roomCount);
    void CalculateRoomPositions();
    void AnalyzeRoomConnections();
    void SpawnRoomContents(const Room& room, const ScalingConfig& scaling);
    void SpawnPokerTable(Vector3 position, const ScalingConfig& scaling);
    void SpawnResources(const Room& room, const ScalingConfig& scaling);
    void BuildWalls(const Room& room);
    void BuildFloorAndCeiling(const Room& room);
    
    // Helper methods
    int FindRoomAtGrid(int gridX, int gridZ) const;

public:
    // Constructor
    LevelGenerator(PhysicsWorld* physicsWorld, DOM* domInstance);
    
    // Generate a complete level
    void GenerateLevel(int level);
    
    // Get player spawn position for this level
    Vector3 GetPlayerSpawnPosition() const;
    
    // Cleanup (should be called before generating new level)
    void Clear();
};

#endif // LEVEL_GENERATOR_HPP
