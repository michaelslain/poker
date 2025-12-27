#ifndef LEVEL_GENERATOR_HPP
#define LEVEL_GENERATOR_HPP

#include "core/object.hpp"
#include "core/physics.hpp"
#include "core/dom.hpp"
#include "core/level_manager.hpp"
#include "raylib.h"
#include <vector>
#include <unordered_map>

// Forward declaration
class RoomVisibilityManager;

// Grid coordinate for room lookup
struct GridCoord;
struct GridCoordHash;

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

    // Async generation state
    enum class GenerationStage {
        IDLE,              // Not generating
        LAYOUT,            // Generating room layout (stages 1-3, fast)
        BUILDING_ROOMS,    // Building rooms incrementally
        SPAWNING_LIGHTS,   // Creating light bulbs
        COMPLETE           // Generation finished
    };

    GenerationStage currentStage;
    int currentRoomIndex;                   // Which room we're currently building
    std::vector<Room> pendingRooms;         // Rooms to build (from GenerateRooms())
    ScalingConfig generationScaling;        // Cached scaling for this generation
    int generationLevel;                    // Level number being generated
    int pokerTablesSpawned;                 // Track poker tables for max limit
    int enemiesSpawned;                     // Track total enemies for max limit
    RoomVisibilityManager* visibilityManager;  // Optional visibility manager for occlusion culling

    std::vector<Room> rooms;

    // Generation parameters
    static constexpr int MIN_ROOMS = 3;
    static constexpr int MAX_ROOMS = 32;  // Matches MAX_LIGHTS for one light per room
    static constexpr int MAX_POKER_TABLES = 3;  // Maximum poker tables per level
    static constexpr int MAX_ENEMIES = 10;      // Maximum total enemies per level (physics optimization)
    static constexpr float ROOM_MIN_SIZE = 8.0f;
    static constexpr float ROOM_MAX_SIZE = 15.0f;
    static constexpr float HALLWAY_WIDTH = 3.0f;
    static constexpr float FLOOR_HEIGHT = 0.0f;
    static constexpr float CEILING_HEIGHT = 7.0f;
    static constexpr float TABLE_HEIGHT = 2.2f;  // Standard poker table height above floor
    
    // Private generation methods
    void GenerateRooms(int roomCount);
    void CalculateRoomPositions();
    void AnalyzeRoomConnections();
    void SpawnRoomContents(const Room& room, const ScalingConfig& scaling, int roomIndex);
    void SpawnPokerTable(Vector3 position, const ScalingConfig& scaling, int roomIndex);
    void SpawnResources(const Room& room, const ScalingConfig& scaling, int roomIndex);
    void BuildWalls(const Room& room, int roomIndex);
    void BuildFloorAndCeiling(const Room& room, int roomIndex);
    
    // Helper methods
    int FindRoomAtGrid(int gridX, int gridZ) const;

public:
    // Constructor
    LevelGenerator(PhysicsWorld* physicsWorld, DOM* domInstance);

    // Generate a complete level (synchronous)
    void GenerateLevel(int level);

    // Async generation methods
    void StartGeneration(int level);                    // Initialize generation state
    bool ContinueGeneration(int roomsPerFrame = 2);    // Build N rooms, returns true if complete
    bool IsGenerating() const { return currentStage != GenerationStage::IDLE; }
    GenerationStage GetGenerationStage() const { return currentStage; }

    // Get player spawn position for this level
    Vector3 GetPlayerSpawnPosition() const;

    // Visibility manager integration
    void SetVisibilityManager(RoomVisibilityManager* vm) { visibilityManager = vm; }
    RoomVisibilityManager* GetVisibilityManager() const { return visibilityManager; }
    std::vector<Room>& GetRooms() { return rooms; }

    // Cleanup (should be called before generating new level)
    void Clear();
};

#endif // LEVEL_GENERATOR_HPP
