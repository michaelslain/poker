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
    Floor* floor = new Floor(floorPos, {ROOM_WIDTH, ROOM_DEPTH}, WHITE, physics);
    dom->AddObject(floor);
    
    // Ceiling (black, unlit)
    Vector3 ceilingPos = {0, CEILING_HEIGHT, 0};
    Ceiling* ceiling = new Ceiling(ceilingPos, {ROOM_WIDTH, ROOM_DEPTH}, BLACK, physics);
    dom->AddObject(ceiling);
    
    // Walls (Wall constructor doesn't take color - uses default)
    float wallHeight = CEILING_HEIGHT - FLOOR_HEIGHT;
    
    // North wall
    Vector3 northPos = {0, FLOOR_HEIGHT + wallHeight / 2.0f, -ROOM_DEPTH / 2.0f};
    Vector3 northSize = {ROOM_WIDTH + WALL_THICKNESS * 2, wallHeight, WALL_THICKNESS};
    Wall* northWall = new Wall(northPos, northSize, physics);
    dom->AddObject(northWall);
    
    // South wall
    Vector3 southPos = {0, FLOOR_HEIGHT + wallHeight / 2.0f, ROOM_DEPTH / 2.0f};
    Vector3 southSize = {ROOM_WIDTH + WALL_THICKNESS * 2, wallHeight, WALL_THICKNESS};
    Wall* southWall = new Wall(southPos, southSize, physics);
    dom->AddObject(southWall);
    
    // West wall
    Vector3 westPos = {-ROOM_WIDTH / 2.0f, FLOOR_HEIGHT + wallHeight / 2.0f, 0};
    Vector3 westSize = {WALL_THICKNESS, wallHeight, ROOM_DEPTH};
    Wall* westWall = new Wall(westPos, westSize, physics);
    dom->AddObject(westWall);
    
    // East wall
    Vector3 eastPos = {ROOM_WIDTH / 2.0f, FLOOR_HEIGHT + wallHeight / 2.0f, 0};
    Vector3 eastSize = {WALL_THICKNESS, wallHeight, ROOM_DEPTH};
    Wall* eastWall = new Wall(eastPos, eastSize, physics);
    dom->AddObject(eastWall);
    
    // Light (sterile white hospital light) - position so chain reaches ceiling
    // Chain extends 1.0 unit up, so light at CEILING_HEIGHT - 1.0 means chain touches ceiling
    Vector3 lightPos = {0, CEILING_HEIGHT - 1.0f, 0};
    LightBulb* light = new LightBulb(lightPos, WHITE);
    dom->AddObject(light);
    
    // Stairs to level 1 (exit hospital)
    Vector3 stairsPos = {0, FLOOR_HEIGHT + 1.0f, ROOM_DEPTH / 2.0f - 2.0f};
    Vector3 stairsSize = {3.0f, 2.0f, 3.0f};
    Stairs* stairs = new Stairs(stairsPos, stairsSize, GRAY, physics);
    dom->AddObject(stairs);
}

Vector3 HospitalScene::GetPlayerSpawnPosition() const {
    // Spawn in center of room
    return {0, 1.8f, 0};
}

void HospitalScene::Clear() {
    // Nothing to clear (DOM handles object cleanup)
}
