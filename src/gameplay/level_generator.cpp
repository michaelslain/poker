#include "gameplay/level_generator.hpp"
#include "core/level_manager.hpp"
#include "world/floor.hpp"
#include "world/ceiling.hpp"
#include "world/wall.hpp"
#include "world/stairs.hpp"
#include "gameplay/poker_table.hpp"
#include "entities/enemy.hpp"
#include "entities/dealer.hpp"
#include "items/chip.hpp"
#include "items/card.hpp"
#include "weapons/pistol.hpp"
#include "substances/weed.hpp"
#include "substances/cocaine.hpp"
#include "substances/molly.hpp"
#include "substances/shrooms.hpp"
#include "substances/vodka.hpp"
#include "substances/salvia.hpp"
#include "substances/fent.hpp"
#include "world/spawner.hpp"
#include "rendering/light_bulb.hpp"
#include <cmath>
#include <algorithm>

LevelGenerator::LevelGenerator(PhysicsWorld* physicsWorld, DOM* domInstance)
    : physics(physicsWorld), dom(domInstance), levelNumber(0)
{
}

void LevelGenerator::GenerateLevel(int level) {
    levelNumber = level;
    rooms.clear();
    hallways.clear();
    
    TraceLog(LOG_INFO, "LEVEL_GEN: Generating level %d", level);
    
    // Get scaling config from level manager
    const ScalingConfig& scaling = LevelManager::GetInstance()->GetScaling();
    
    // Determine number of rooms based on level
    int roomCount = MIN_ROOMS + (level / 2);  // More rooms at higher levels
    roomCount = std::min(roomCount, MAX_ROOMS);
    
    TraceLog(LOG_INFO, "LEVEL_GEN: Creating %d rooms", roomCount);
    
    // Generate room layout
    GenerateRooms(roomCount);
    
    // Generate hallways connecting rooms
    GenerateHallways();
    
    // Build geometry and spawn contents for each room
    TraceLog(LOG_INFO, "LEVEL_GEN: Building %d rooms", (int)rooms.size());
    for (size_t i = 0; i < rooms.size(); i++) {
        const Room& room = rooms[i];
        TraceLog(LOG_INFO, "LEVEL_GEN: Room %d at (%.1f, %.1f), size (%.1f, %.1f), hasTable=%d, hasStairs=%d", 
                 (int)i, room.position.x, room.position.y, room.size.x, room.size.y, 
                 room.hasPokerTable, room.hasStairs);
        BuildFloorAndCeiling(room);
        BuildWalls(room);
        SpawnRoomContents(room, scaling);
    }
    
    // Build hallway walls
    for (const Hallway& hallway : hallways) {
        BuildHallwayWalls(hallway);
    }
    
    // Spawn lights throughout level
    for (const Room& room : rooms) {
        // One light per room at center - chain extends 1.0 up, so this makes it touch ceiling
        Vector3 lightPos = {room.position.x, CEILING_HEIGHT - 1.0f, room.position.y};
        LightBulb* light = new LightBulb(lightPos, YELLOW);
        dom->AddObject(light);
    }
}

void LevelGenerator::GenerateRooms(int roomCount) {
    // Simple grid-based room generation with random sizes
    // Rooms are placed in a rough grid pattern to avoid overlap
    
    // Place rooms in a line for now (simpler, rooms are connected)
    float spacing = 12.0f;  // Distance between room centers
    
    for (int i = 0; i < roomCount; i++) {
        Room room;
        
        // Place rooms in a line along X axis
        room.position.x = i * spacing;
        room.position.y = 0.0f;
        
        // Random size
        room.size.x = GetRandomValue(static_cast<int>(ROOM_MIN_SIZE * 100), 
                                      static_cast<int>(ROOM_MAX_SIZE * 100)) / 100.0f;
        room.size.y = GetRandomValue(static_cast<int>(ROOM_MIN_SIZE * 100), 
                                      static_cast<int>(ROOM_MAX_SIZE * 100)) / 100.0f;
        
        // First room is start room
        room.isStartRoom = (i == 0);
        
        // Last room has stairs
        room.hasStairs = (i == roomCount - 1);
        
        // 60% chance to have poker table (but not in start room or stairs room)
        room.hasPokerTable = !room.isStartRoom && !room.hasStairs && (GetRandomValue(0, 100) < 60);
        
        rooms.push_back(room);
    }
}

void LevelGenerator::GenerateHallways() {
    // Connect each room to the next room in sequence
    // This ensures all rooms are connected
    
    for (size_t i = 0; i < rooms.size() - 1; i++) {
        Hallway hallway;
        hallway.start = FindClosestPoint(rooms[i], rooms[i + 1]);
        hallway.end = FindClosestPoint(rooms[i + 1], rooms[i]);
        hallway.width = HALLWAY_WIDTH;
        hallways.push_back(hallway);
    }
}

void LevelGenerator::SpawnRoomContents(const Room& room, const ScalingConfig& scaling) {
    Vector3 roomCenter = {room.position.x, FLOOR_HEIGHT, room.position.y};
    
    // Spawn poker table if room has one
    if (room.hasPokerTable) {
        SpawnPokerTable(roomCenter, scaling);
    }
    
    // Spawn stairs if room has them
    if (room.hasStairs) {
        Vector3 stairsPos = {room.position.x, FLOOR_HEIGHT + 1.0f, room.position.y};
        Vector3 stairsSize = {3.0f, 2.0f, 3.0f};
        Stairs* stairs = new Stairs(stairsPos, stairsSize, DARKGRAY, physics);
        dom->AddObject(stairs);
    }
    
    // Spawn resources (chips, weapons, substances)
    if (!room.isStartRoom) {  // Don't spawn resources in start room
        SpawnResources(room, scaling);
    }
}

void LevelGenerator::SpawnPokerTable(Vector3 position, const ScalingConfig& scaling) {
    // Create poker table
    Vector3 tableSize = {4.0f, 0.2f, 2.5f};
    PokerTable* table = new PokerTable(position, tableSize, DARKBROWN, physics);
    dom->AddObject(table);
    
    // Spawn enemies around table
    int enemyCount = GetRandomValue(scaling.minEnemiesPerTable, scaling.maxEnemiesPerTable);
    
    for (int i = 0; i < enemyCount; i++) {
        Vector3 enemySpawnPos = {position.x + i * 0.5f, position.y, position.z};
        Enemy* enemy = new Enemy(enemySpawnPos, "Enemy " + std::to_string(i + 1));
        
        // Find and seat enemy at table
        int seatIndex = table->FindClosestOpenSeat(enemySpawnPos);
        if (seatIndex >= 0) {  // Valid seat found
            table->SeatPerson(enemy, seatIndex);
        }
        
        dom->AddObject(enemy);
    }
    
    // Spawn dealer
    Vector3 dealerPos = {position.x, position.y, position.z - 3.0f};  // Behind table
    Dealer* dealer = new Dealer(dealerPos, "Dealer");
    dom->AddObject(dealer);
}

void LevelGenerator::SpawnResources(const Room& room, const ScalingConfig& scaling) {
    // Spawn resources based on scaling (fewer resources at higher levels)
    
    // Chips - always spawn some
    int chipSpawnCount = static_cast<int>(3 * scaling.resourceSpawnRate);
    if (chipSpawnCount > 0) {
        Vector3 chipSpawnPos = {
            room.position.x + GetRandomValue(-100, 100) / 50.0f,
            2.0f,
            room.position.y + GetRandomValue(-100, 100) / 50.0f
        };
        
        int chipValue = (GetRandomValue(0, 100) < 50) ? 5 : 25;  // Mostly low-value chips
        Spawner* chipSpawner = new Spawner(chipSpawnPos, 1.5f, new Chip(chipValue, {0,0,0}, nullptr), chipSpawnCount);
        dom->AddObject(chipSpawner);
    }
    
    // Weapons - rare
    if (GetRandomValue(0, 100) < static_cast<int>(30 * scaling.resourceSpawnRate)) {
        Vector3 weaponPos = {
            room.position.x + GetRandomValue(-100, 100) / 50.0f,
            2.0f,
            room.position.y + GetRandomValue(-100, 100) / 50.0f
        };
        Pistol* pistol = new Pistol(weaponPos, physics);
        dom->AddObject(pistol);
    }
    
    // Substances - random selection
    if (GetRandomValue(0, 100) < static_cast<int>(40 * scaling.resourceSpawnRate)) {
        Vector3 substancePos = {
            room.position.x + GetRandomValue(-100, 100) / 50.0f,
            2.0f,
            room.position.y + GetRandomValue(-100, 100) / 50.0f
        };
        
        // Random substance type
        int substanceType = GetRandomValue(0, 6);
        Object* substance = nullptr;
        
        switch (substanceType) {
            case 0: substance = new Weed(substancePos, physics); break;
            case 1: substance = new Cocaine(substancePos, physics); break;
            case 2: substance = new Molly(substancePos, physics); break;
            case 3: substance = new Shrooms(substancePos, physics); break;
            case 4: substance = new Vodka(substancePos, physics); break;
            case 5: substance = new Salvia(substancePos, physics); break;
            case 6: substance = new Fent(substancePos, physics); break;
        }
        
        if (substance) {
            dom->AddObject(substance);
        }
    }
}

void LevelGenerator::BuildWalls(const Room& room) {
    // Build walls around room, but leave openings for hallways
    // For now: only build North and South walls (leave East/West open for linear progression)
    float halfWidth = room.size.x / 2.0f;
    float halfDepth = room.size.y / 2.0f;
    float wallHeight = CEILING_HEIGHT - FLOOR_HEIGHT;
    
    // North wall (Wall constructor doesn't take color - uses default)
    Vector3 northPos = {room.position.x, FLOOR_HEIGHT + wallHeight / 2.0f, room.position.y - halfDepth};
    Vector3 northSize = {room.size.x + WALL_THICKNESS * 2, wallHeight, WALL_THICKNESS};
    Wall* northWall = new Wall(northPos, northSize, physics);
    dom->AddObject(northWall);
    
    // South wall
    Vector3 southPos = {room.position.x, FLOOR_HEIGHT + wallHeight / 2.0f, room.position.y + halfDepth};
    Vector3 southSize = {room.size.x + WALL_THICKNESS * 2, wallHeight, WALL_THICKNESS};
    Wall* southWall = new Wall(southPos, southSize, physics);
    dom->AddObject(southWall);
    
    // Skip East/West walls to allow linear progression between rooms
    // TODO: Add proper doorway system when implementing complex room layouts
}

void LevelGenerator::BuildHallwayWalls(const Hallway& hallway) {
    // Build walls along hallway
    Vector2 direction = {hallway.end.x - hallway.start.x, hallway.end.y - hallway.start.y};
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Normalize direction
    direction.x /= length;
    direction.y /= length;
    
    // Perpendicular direction for wall placement
    Vector2 perpendicular = {-direction.y, direction.x};
    
    // Center of hallway
    Vector2 center = {
        (hallway.start.x + hallway.end.x) / 2.0f,
        (hallway.start.y + hallway.end.y) / 2.0f
    };
    
    float wallHeight = CEILING_HEIGHT - FLOOR_HEIGHT;
    
    // Left wall
    Vector3 leftPos = {
        center.x + perpendicular.x * hallway.width / 2.0f,
        FLOOR_HEIGHT + wallHeight / 2.0f,
        center.y + perpendicular.y * hallway.width / 2.0f
    };
    Vector3 leftSize = {length, wallHeight, WALL_THICKNESS};
    Wall* leftWall = new Wall(leftPos, leftSize, physics);
    
    // Rotate wall to align with hallway
    float angle = std::atan2(direction.y, direction.x) * RAD2DEG;
    leftWall->rotation.y = angle;
    
    dom->AddObject(leftWall);
    
    // Right wall
    Vector3 rightPos = {
        center.x - perpendicular.x * hallway.width / 2.0f,
        FLOOR_HEIGHT + wallHeight / 2.0f,
        center.y - perpendicular.y * hallway.width / 2.0f
    };
    Vector3 rightSize = {length, wallHeight, WALL_THICKNESS};
    Wall* rightWall = new Wall(rightPos, rightSize, physics);
    rightWall->rotation.y = angle;
    dom->AddObject(rightWall);
}

void LevelGenerator::BuildFloorAndCeiling(const Room& room) {
    // Floor
    Vector3 floorPos = {room.position.x, FLOOR_HEIGHT, room.position.y};
    Floor* floor = new Floor(floorPos, {room.size.x, room.size.y}, GRAY, physics);
    dom->AddObject(floor);
    
    // Ceiling
    Vector3 ceilingPos = {room.position.x, CEILING_HEIGHT, room.position.y};
    Ceiling* ceiling = new Ceiling(ceilingPos, {room.size.x, room.size.y}, DARKGRAY, physics);
    dom->AddObject(ceiling);
}

bool LevelGenerator::RoomsOverlap(const Room& a, const Room& b) const {
    float aLeft = a.position.x - a.size.x / 2.0f;
    float aRight = a.position.x + a.size.x / 2.0f;
    float aTop = a.position.y - a.size.y / 2.0f;
    float aBottom = a.position.y + a.size.y / 2.0f;
    
    float bLeft = b.position.x - b.size.x / 2.0f;
    float bRight = b.position.x + b.size.x / 2.0f;
    float bTop = b.position.y - b.size.y / 2.0f;
    float bBottom = b.position.y + b.size.y / 2.0f;
    
    return !(aRight < bLeft || aLeft > bRight || aBottom < bTop || aTop > bBottom);
}

Vector2 LevelGenerator::FindClosestPoint(const Room& from, const Room& to) const {
    // Find closest point on edge of 'from' room toward 'to' room
    Vector2 direction = {to.position.x - from.position.x, to.position.y - from.position.y};
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction.x /= length;
    direction.y /= length;
    
    // Move to edge of room in that direction
    Vector2 point = {
        from.position.x + direction.x * from.size.x / 2.0f,
        from.position.y + direction.y * from.size.y / 2.0f
    };
    
    return point;
}

Vector3 LevelGenerator::GetPlayerSpawnPosition() const {
    if (rooms.empty()) {
        return {0, 1.8f, 0};  // Default spawn
    }
    
    // Spawn in first room (start room)
    const Room& startRoom = rooms[0];
    return {startRoom.position.x, 1.8f, startRoom.position.y};
}

void LevelGenerator::Clear() {
    rooms.clear();
    hallways.clear();
}
