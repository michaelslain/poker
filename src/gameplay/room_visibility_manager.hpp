#ifndef ROOM_VISIBILITY_MANAGER_HPP
#define ROOM_VISIBILITY_MANAGER_HPP

#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <queue>

// Forward declaration
struct Room;

// Grid coordinate for room lookup
struct GridCoord {
    int x, z;

    bool operator==(const GridCoord& other) const {
        return x == other.x && z == other.z;
    }
};

// Hash function for GridCoord
struct GridCoordHash {
    size_t operator()(const GridCoord& coord) const {
        return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.z) << 1);
    }
};

// Room visibility manager for occlusion culling
// Tracks object-to-room mapping and calculates visible room sets via BFS
class RoomVisibilityManager {
private:
    std::vector<Room>* rooms;                           // Reference to LevelGenerator rooms
    std::unordered_map<int, int> objectToRoom;          // Object ID -> Room index
    std::vector<std::vector<int>> roomObjects;          // Room index -> Object IDs list
    std::unordered_map<GridCoord, int, GridCoordHash> gridToRoom;  // Grid lookup O(1)
    bool initialized;

public:
    RoomVisibilityManager();

    // Setup
    void Initialize(std::vector<Room>* roomsPtr);
    void Clear();
    bool IsInitialized() const { return initialized; }

    // Object registration
    void AddObjectToRoom(int objectID, int roomIndex);

    // Visibility queries
    int FindRoomContainingPoint(Vector3 point) const;
    std::vector<int> GetVisibleRooms(Vector3 playerPos, int maxDepth = 1) const;
    const std::vector<int>& GetRoomObjects(int roomIndex) const;

    // Grid lookup optimization
    void BuildGridLookup();
    int FindRoomAtGrid(int gridX, int gridZ) const;

private:
    // Helper for BFS visibility calculation
    void ExpandVisibleRooms(int roomIdx, int depth, int maxDepth,
                           std::vector<bool>& visited,
                           std::queue<std::pair<int, int>>& queue) const;
};

#endif // ROOM_VISIBILITY_MANAGER_HPP
