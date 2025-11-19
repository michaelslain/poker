#include "scenes/hospital_scene.hpp"
#include "world/floor.hpp"
#include "world/ceiling.hpp"
#include "world/wall.hpp"
#include "world/stairs.hpp"
#include "rendering/light_bulb.hpp"

HospitalScene::HospitalScene(PhysicsWorld* physicsWorld, DOM* domInstance)
    : physics(physicsWorld), dom(domInstance)
{
}

void HospitalScene::Generate() {
    // TODO: Future cutscene/narrative intro
    // Ideas for future implementation:
    // - Hospital bed in center
    // - Medical equipment scattered around
    // - Flickering fluorescent lights
    // - Dark/eerie atmosphere
    // - Text prompts explaining backstory
    // - Maybe a doctor NPC?
    // - Tutorial elements (movement, interaction prompts)
    
    // For now: Simple empty room with floor, ceiling, walls, and stairs
    
    // Floor
    Vector3 floorPos = {0, FLOOR_HEIGHT, 0};
    Floor* floor = new Floor(floorPos, {ROOM_WIDTH, ROOM_DEPTH}, WHITE);
    dom->AddObject(floor);
    
    // Ceiling (black, unlit)
    Vector3 ceilingPos = {0, CEILING_HEIGHT, 0};
    Ceiling* ceiling = new Ceiling(ceilingPos, {ROOM_WIDTH, ROOM_DEPTH}, BLACK);
    dom->AddObject(ceiling);
    
    // Walls - now 2D planes with no thickness
    float wallHeight = CEILING_HEIGHT - FLOOR_HEIGHT;
    
    // North wall (runs along X axis, faces +Z)
    Vector3 northPos = {0, FLOOR_HEIGHT + wallHeight / 2.0f, -ROOM_DEPTH / 2.0f};
    Vector3 northSize = {ROOM_WIDTH, wallHeight, 0.0f};  // 2D plane
    Wall* northWall = new Wall(northPos, northSize);
    dom->AddObject(northWall);
    
    // South wall (runs along X axis, faces -Z)
    Vector3 southPos = {0, FLOOR_HEIGHT + wallHeight / 2.0f, ROOM_DEPTH / 2.0f};
    Vector3 southSize = {ROOM_WIDTH, wallHeight, 0.0f};  // 2D plane
    Wall* southWall = new Wall(southPos, southSize);
    dom->AddObject(southWall);
    
    // West wall (runs along Z axis, faces +X) - rotated 90 degrees
    Vector3 westPos = {-ROOM_WIDTH / 2.0f, FLOOR_HEIGHT + wallHeight / 2.0f, 0};
    Vector3 westSize = {ROOM_DEPTH, wallHeight, 0.0f};  // 2D plane
    Wall* westWall = new Wall(westPos, westSize);
    westWall->rotation.y = 90.0f;  // Rotate to face along Z axis
    westWall->needsColliderUpdate = true;  // Flag to update collider after rotation
    dom->AddObject(westWall);
    
    // East wall (runs along Z axis, faces -X) - rotated 90 degrees
    Vector3 eastPos = {ROOM_WIDTH / 2.0f, FLOOR_HEIGHT + wallHeight / 2.0f, 0};
    Vector3 eastSize = {ROOM_DEPTH, wallHeight, 0.0f};  // 2D plane
    Wall* eastWall = new Wall(eastPos, eastSize);
    eastWall->rotation.y = 90.0f;  // Rotate to face along Z axis
    eastWall->needsColliderUpdate = true;  // Flag to update collider after rotation
    dom->AddObject(eastWall);
    
    // Light (sterile white hospital light) - position so chain reaches ceiling
    // Chain extends 1.0 unit up, so light at CEILING_HEIGHT - 1.0 means chain touches ceiling
    Vector3 lightPos = {0, CEILING_HEIGHT - 1.0f, 0};
    LightBulb* light = new LightBulb(lightPos, WHITE);
    dom->AddObject(light);
    
    // Stairs to level 1 (exit hospital)
    Vector3 stairsPos = {0, FLOOR_HEIGHT + 1.0f, ROOM_DEPTH / 2.0f - 2.0f};
    Vector3 stairsSize = {3.0f, 2.0f, 3.0f};
    Stairs* stairs = new Stairs(stairsPos, stairsSize, GRAY);
    dom->AddObject(stairs);
}

Vector3 HospitalScene::GetPlayerSpawnPosition() const {
    // NEW SYSTEM: Player spawn at feet level, slightly above floor to avoid penetration
    return {0, FLOOR_HEIGHT + 0.01f, 0};
}

void HospitalScene::Clear() {
    // Nothing to clear (DOM handles object cleanup)
}
