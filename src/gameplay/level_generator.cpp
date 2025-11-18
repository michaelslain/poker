#include "gameplay/level_generator.hpp"
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
{}

void LevelGenerator::GenerateLevel(int level) {
    levelNumber = level;
    rooms.clear();
    
    TraceLog(LOG_INFO, "LEVEL_GEN: Generating level %d", level);
    
    // Get scaling config from level manager
    const ScalingConfig& scaling = LevelManager::GetInstance()->GetScaling();
    
    // Determine number of rooms based on level
    int roomCount = MIN_ROOMS + (level / 2);  // More rooms at higher levels
    roomCount = std::min(roomCount, MAX_ROOMS);
    
    TraceLog(LOG_INFO, "LEVEL_GEN: Creating %d rooms", roomCount);
    
    // Generate room layout
    GenerateRooms(roomCount);
    
    // Calculate aligned world positions for all rooms
    CalculateRoomPositions();
    
    // Analyze which rooms connect to each other
    AnalyzeRoomConnections();
    
    // Build geometry and spawn contents for each room
    TraceLog(LOG_INFO, "LEVEL_GEN: Building %d rooms", (int)rooms.size());
    for (size_t i = 0; i < rooms.size(); i++) {
        const Room& room = rooms[i];
        TraceLog(LOG_INFO, "LEVEL_GEN: Room %d at (%.1f, %.1f), grid (%d, %d), size (%.1f, %.1f), hasTable=%d, hasStairs=%d",
                 (int)i, room.position.x, room.position.y, room.gridX, room.gridZ,
                 room.size.x, room.size.y, room.hasPokerTable, room.hasStairs);
        BuildFloorAndCeiling(room);
        BuildWalls(room);
        SpawnRoomContents(room, scaling);
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
    // Grid-based room generation using random walk algorithm
    // Rooms can branch out in different directions creating organic layouts
    // Key constraint: Rooms that connect on an axis must share the same size on that axis
    // Example: Room to the North must have same width, but can have different depth
    
    // Start at origin
    int currentGridX = 0;
    int currentGridZ = 0;
    
    for (int i = 0; i < roomCount; i++) {
        Room room;
        
        // Grid position
        room.gridX = currentGridX;
        room.gridZ = currentGridZ;
        
        // Determine size based on existing neighbors
        // If we have a neighbor to North/South, we must match their width (size.x)
        // If we have a neighbor to East/West, we must match their depth (size.y)
        
        int northNeighborIdx = FindRoomAtGrid(currentGridX, currentGridZ - 1);
        int southNeighborIdx = FindRoomAtGrid(currentGridX, currentGridZ + 1);
        int eastNeighborIdx = FindRoomAtGrid(currentGridX + 1, currentGridZ);
        int westNeighborIdx = FindRoomAtGrid(currentGridX - 1, currentGridZ);
        
        // Determine width (size.x)
        if (northNeighborIdx != -1) {
            // Match north neighbor's width
            room.size.x = rooms[northNeighborIdx].size.x;
        } else if (southNeighborIdx != -1) {
            // Match south neighbor's width
            room.size.x = rooms[southNeighborIdx].size.x;
        } else {
            // No North/South neighbor - random width
            room.size.x = GetRandomValue(static_cast<int>(ROOM_MIN_SIZE * 100),
                                          static_cast<int>(ROOM_MAX_SIZE * 100)) / 100.0f;
        }
        
        // Determine depth (size.y)
        if (eastNeighborIdx != -1) {
            // Match east neighbor's depth
            room.size.y = rooms[eastNeighborIdx].size.y;
        } else if (westNeighborIdx != -1) {
            // Match west neighbor's depth
            room.size.y = rooms[westNeighborIdx].size.y;
        } else {
            // No East/West neighbor - random depth
            room.size.y = GetRandomValue(static_cast<int>(ROOM_MIN_SIZE * 100),
                                          static_cast<int>(ROOM_MAX_SIZE * 100)) / 100.0f;
        }
        
        // Calculate world position based on neighboring rooms to align edges
        // We'll adjust position after all rooms are placed
        room.position.x = 0;  // Placeholder
        room.position.y = 0;  // Placeholder
        
        // First room is start room
        room.isStartRoom = (i == 0);
        
        // Last room has stairs
        room.hasStairs = (i == roomCount - 1);
        
        // 60% chance to have poker table (but not in start room or stairs room)
        room.hasPokerTable = !room.isStartRoom && !room.hasStairs && (GetRandomValue(0, 100) < 60);
        
        // Initialize connections to false
        room.connectsNorth = false;
        room.connectsSouth = false;
        room.connectsEast = false;
        room.connectsWest = false;
        
        rooms.push_back(room);
        
        // Choose next grid position (random walk)
        if (i < roomCount - 1) {
            // Pick a random direction that doesn't overlap with existing room
            int attempts = 0;
            int nextGridX, nextGridZ;
            
            do {
                int direction = GetRandomValue(0, 3);
                nextGridX = currentGridX;
                nextGridZ = currentGridZ;
                
                switch (direction) {
                    case 0: nextGridX++; break;  // East
                    case 1: nextGridX--; break;  // West
                    case 2: nextGridZ++; break;  // South
                    case 3: nextGridZ--; break;  // North
                }
                
                attempts++;
                // Prevent infinite loop
                if (attempts > 20) {
                    // Force progress in positive X direction
                    nextGridX = currentGridX + 1;
                    nextGridZ = currentGridZ;
                    break;
                }
            } while (FindRoomAtGrid(nextGridX, nextGridZ) != -1);
            
            currentGridX = nextGridX;
            currentGridZ = nextGridZ;
        }
    }
}

void LevelGenerator::CalculateRoomPositions() {
    // Calculate world positions for rooms so their edges align perfectly
    // Strategy: Start from first room at origin, then place each connected room
    // by aligning its edge with the previous room's edge
    
    if (rooms.empty()) return;
    
    // First room at origin
    rooms[0].position.x = 0.0f;
    rooms[0].position.y = 0.0f;
    
    // Process rooms using breadth-first search from origin
    // This ensures we place rooms based on their neighbors
    std::vector<bool> placed(rooms.size(), false);
    placed[0] = true;
    
    bool anyPlaced = true;
    while (anyPlaced) {
        anyPlaced = false;
        
        for (size_t i = 0; i < rooms.size(); i++) {
            if (!placed[i]) continue;
            
            Room& currentRoom = rooms[i];
            
            // Try to place all neighbors of this room
            for (size_t j = 0; j < rooms.size(); j++) {
                if (placed[j]) continue;
                
                Room& neighborRoom = rooms[j];
                
                // Check if rooms are grid neighbors
                int dx = neighborRoom.gridX - currentRoom.gridX;
                int dz = neighborRoom.gridZ - currentRoom.gridZ;
                
                if (abs(dx) + abs(dz) == 1) {  // Adjacent on grid
                    // Place neighbor room by aligning edges
                    if (dx == 1) {  // Neighbor is to the East
                        neighborRoom.position.x = currentRoom.position.x + currentRoom.size.x / 2.0f + neighborRoom.size.x / 2.0f;
                        neighborRoom.position.y = currentRoom.position.y;
                    } else if (dx == -1) {  // Neighbor is to the West
                        neighborRoom.position.x = currentRoom.position.x - currentRoom.size.x / 2.0f - neighborRoom.size.x / 2.0f;
                        neighborRoom.position.y = currentRoom.position.y;
                    } else if (dz == 1) {  // Neighbor is to the South
                        neighborRoom.position.x = currentRoom.position.x;
                        neighborRoom.position.y = currentRoom.position.y + currentRoom.size.y / 2.0f + neighborRoom.size.y / 2.0f;
                    } else if (dz == -1) {  // Neighbor is to the North
                        neighborRoom.position.x = currentRoom.position.x;
                        neighborRoom.position.y = currentRoom.position.y - currentRoom.size.y / 2.0f - neighborRoom.size.y / 2.0f;
                    }
                    
                    placed[j] = true;
                    anyPlaced = true;
                }
            }
        }
    }
}

void LevelGenerator::AnalyzeRoomConnections() {
    // Check each room's neighbors and mark connections
    for (size_t i = 0; i < rooms.size(); i++) {
        Room& room = rooms[i];
        
        // Check North (gridZ - 1)
        if (FindRoomAtGrid(room.gridX, room.gridZ - 1) != -1) {
            room.connectsNorth = true;
        }
        
        // Check South (gridZ + 1)
        if (FindRoomAtGrid(room.gridX, room.gridZ + 1) != -1) {
            room.connectsSouth = true;
        }
        
        // Check East (gridX + 1)
        if (FindRoomAtGrid(room.gridX + 1, room.gridZ) != -1) {
            room.connectsEast = true;
        }
        
        // Check West (gridX - 1)
        if (FindRoomAtGrid(room.gridX - 1, room.gridZ) != -1) {
            room.connectsWest = true;
        }
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
    // Create poker table at standardized height
    Vector3 tablePos = position;
    tablePos.y = FLOOR_HEIGHT + TABLE_HEIGHT;
    Vector3 tableSize = {4.0f, 0.2f, 2.5f};
    PokerTable* table = new PokerTable(tablePos, tableSize, DARKBROWN, physics);
    dom->AddObject(table);
    
    // Spawn enemies around table
    int enemyCount = GetRandomValue(scaling.minEnemiesPerTable, scaling.maxEnemiesPerTable);
    
    for (int i = 0; i < enemyCount; i++) {
        // Spawn enemies at floor level, not at table height
        Vector3 enemySpawnPos = {position.x + i * 0.5f, 1.8f, position.z};
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
    // Build walls around room with doorway openings where rooms connect
    // Walls are 2D planes with no thickness - only width (X) and height (Y)
    // Since rooms share dimensions on connecting axes, walls align perfectly
    float halfWidth = room.size.x / 2.0f;
    float halfDepth = room.size.y / 2.0f;
    float wallHeight = CEILING_HEIGHT - FLOOR_HEIGHT;
    
    // North wall (negative Z) - skip if connects north
    if (!room.connectsNorth) {
        Vector3 northPos = {room.position.x, FLOOR_HEIGHT + wallHeight / 2.0f, room.position.y - halfDepth};
        Vector3 northSize = {room.size.x, wallHeight, 0.0f};
        Wall* northWall = new Wall(northPos, northSize, physics);
        dom->AddObject(northWall);
    }
    
    // South wall (positive Z) - skip if connects south
    if (!room.connectsSouth) {
        Vector3 southPos = {room.position.x, FLOOR_HEIGHT + wallHeight / 2.0f, room.position.y + halfDepth};
        Vector3 southSize = {room.size.x, wallHeight, 0.0f};
        Wall* southWall = new Wall(southPos, southSize, physics);
        dom->AddObject(southWall);
    }
    
    // East wall (positive X) - skip if connects east
    if (!room.connectsEast) {
        Vector3 eastPos = {room.position.x + halfWidth, FLOOR_HEIGHT + wallHeight / 2.0f, room.position.y};
        Vector3 eastSize = {room.size.y, wallHeight, 0.0f};  // Note: width is room depth
        Wall* eastWall = new Wall(eastPos, eastSize, physics);
        eastWall->rotation.y = 90.0f;  // Rotate to face East/West
        eastWall->needsColliderUpdate = true;  // Mark for collider update
        dom->AddObject(eastWall);
    }
    
    // West wall (negative X) - skip if connects west
    if (!room.connectsWest) {
        Vector3 westPos = {room.position.x - halfWidth, FLOOR_HEIGHT + wallHeight / 2.0f, room.position.y};
        Vector3 westSize = {room.size.y, wallHeight, 0.0f};  // Note: width is room depth
        Wall* westWall = new Wall(westPos, westSize, physics);
        westWall->rotation.y = 90.0f;  // Rotate to face East/West
        westWall->needsColliderUpdate = true;  // Mark for collider update
        dom->AddObject(westWall);
    }
}

void LevelGenerator::BuildFloorAndCeiling(const Room& room) {
    // Floor - extremely dark maroon color
    Vector3 floorPos = {room.position.x, FLOOR_HEIGHT, room.position.y};
    Color darkMaroon = {20, 2, 2, 255};
    Floor* floor = new Floor(floorPos, {room.size.x, room.size.y}, darkMaroon, physics);
    dom->AddObject(floor);
    
    // Ceiling
    Vector3 ceilingPos = {room.position.x, CEILING_HEIGHT, room.position.y};
    Ceiling* ceiling = new Ceiling(ceilingPos, {room.size.x, room.size.y}, DARKGRAY, physics);
    dom->AddObject(ceiling);
}

int LevelGenerator::FindRoomAtGrid(int gridX, int gridZ) const {
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i].gridX == gridX && rooms[i].gridZ == gridZ) {
            return static_cast<int>(i);
        }
    }
    return -1;  // No room found
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
}
