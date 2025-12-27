#include "gameplay/room_visibility_manager.hpp"
#include "gameplay/level_generator.hpp"  // For Room struct
#include "raymath.h"
#include <algorithm>

RoomVisibilityManager::RoomVisibilityManager()
    : rooms(nullptr), initialized(false)
{}

void RoomVisibilityManager::Initialize(std::vector<Room>* roomsPtr) {
    rooms = roomsPtr;
    initialized = (rooms != nullptr && !rooms->empty());

    if (initialized) {
        // Allocate space for room objects
        roomObjects.clear();
        roomObjects.resize(rooms->size());

        // Build grid lookup for O(1) neighbor queries
        BuildGridLookup();

        TraceLog(LOG_INFO, "ROOM_VISIBILITY: Initialized with %d rooms", (int)rooms->size());
    }
}

void RoomVisibilityManager::Clear() {
    objectToRoom.clear();
    roomObjects.clear();
    gridToRoom.clear();
    rooms = nullptr;
    initialized = false;
}

void RoomVisibilityManager::AddObjectToRoom(int objectID, int roomIndex) {
    if (!initialized || roomIndex < 0 || roomIndex >= (int)rooms->size()) {
        return;  // Invalid room index
    }

    objectToRoom[objectID] = roomIndex;
    roomObjects[roomIndex].push_back(objectID);
}

int RoomVisibilityManager::FindRoomContainingPoint(Vector3 point) const {
    if (!initialized) return -1;

    // Test point against all room AABBs (X-Z plane)
    for (int i = 0; i < (int)rooms->size(); i++) {
        const Room& room = (*rooms)[i];
        float halfWidth = room.size.x / 2.0f;
        float halfDepth = room.size.y / 2.0f;

        // AABB test on X-Z plane
        if (point.x >= room.position.x - halfWidth &&
            point.x <= room.position.x + halfWidth &&
            point.z >= room.position.y - halfDepth &&
            point.z <= room.position.y + halfDepth) {
            return i;  // Found containing room
        }
    }

    return -1;  // Not in any room
}

std::vector<int> RoomVisibilityManager::GetVisibleRooms(Vector3 playerPos, int maxDepth) const {
    std::vector<int> visible;

    if (!initialized) {
        return visible;  // Empty list
    }

    // Find player's current room
    int currentRoomIndex = FindRoomContainingPoint(playerPos);
    if (currentRoomIndex == -1) {
        return visible;  // Player not in any room, return empty
    }

    // BFS from current room through connections
    std::vector<bool> visited(rooms->size(), false);
    std::queue<std::pair<int, int>> queue;  // (room index, depth)

    queue.push({currentRoomIndex, 0});
    visited[currentRoomIndex] = true;

    while (!queue.empty()) {
        auto [roomIdx, depth] = queue.front();
        queue.pop();

        visible.push_back(roomIdx);

        if (depth >= maxDepth) {
            continue;  // Don't expand further
        }

        // Expand to connected neighbors
        ExpandVisibleRooms(roomIdx, depth, maxDepth, visited, queue);
    }

    return visible;
}

const std::vector<int>& RoomVisibilityManager::GetRoomObjects(int roomIndex) const {
    static const std::vector<int> emptyList;

    if (!initialized || roomIndex < 0 || roomIndex >= (int)roomObjects.size()) {
        return emptyList;
    }

    return roomObjects[roomIndex];
}

void RoomVisibilityManager::BuildGridLookup() {
    if (!initialized) return;

    gridToRoom.clear();

    for (int i = 0; i < (int)rooms->size(); i++) {
        const Room& room = (*rooms)[i];
        GridCoord coord = {room.gridX, room.gridZ};
        gridToRoom[coord] = i;
    }

    TraceLog(LOG_INFO, "ROOM_VISIBILITY: Built grid lookup with %d entries", (int)gridToRoom.size());
}

int RoomVisibilityManager::FindRoomAtGrid(int gridX, int gridZ) const {
    if (!initialized) return -1;

    GridCoord coord = {gridX, gridZ};
    auto it = gridToRoom.find(coord);

    if (it != gridToRoom.end()) {
        return it->second;
    }

    return -1;  // No room at this grid position
}

void RoomVisibilityManager::ExpandVisibleRooms(int roomIdx, int depth, int maxDepth,
                                                std::vector<bool>& visited,
                                                std::queue<std::pair<int, int>>& queue) const {
    const Room& room = (*rooms)[roomIdx];

    // Check all four directions for connected neighbors
    if (room.connectsNorth) {
        int neighbor = FindRoomAtGrid(room.gridX, room.gridZ - 1);
        if (neighbor != -1 && !visited[neighbor]) {
            visited[neighbor] = true;
            queue.push({neighbor, depth + 1});
        }
    }

    if (room.connectsSouth) {
        int neighbor = FindRoomAtGrid(room.gridX, room.gridZ + 1);
        if (neighbor != -1 && !visited[neighbor]) {
            visited[neighbor] = true;
            queue.push({neighbor, depth + 1});
        }
    }

    if (room.connectsEast) {
        int neighbor = FindRoomAtGrid(room.gridX + 1, room.gridZ);
        if (neighbor != -1 && !visited[neighbor]) {
            visited[neighbor] = true;
            queue.push({neighbor, depth + 1});
        }
    }

    if (room.connectsWest) {
        int neighbor = FindRoomAtGrid(room.gridX - 1, room.gridZ);
        if (neighbor != -1 && !visited[neighbor]) {
            visited[neighbor] = true;
            queue.push({neighbor, depth + 1});
        }
    }
}
