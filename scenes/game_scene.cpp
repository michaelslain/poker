#include "game_scene.hpp"
#include "../include/player.hpp"
#include "../include/enemy.hpp"
#include "../include/floor.hpp"
#include "../include/ceiling.hpp"
#include "../include/wall.hpp"
#include "../include/light_bulb.hpp"
#include "../include/poker_table.hpp"
#include "../include/spawner.hpp"
#include "../include/chip.hpp"
#include "../include/card.hpp"
#include "../include/pistol.hpp"

Scene* CreateGameScene(PhysicsWorld* physics) {
    std::vector<Object*> objects;
    
    // Create player
    Player* player = new Player({0.0f, 0.0f, 0.0f}, physics, "Player");
    
    // Give player starting chips (500 chips)
    Inventory* playerInv = player->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        playerInv->AddItem(chip);
    }
    objects.push_back(player);
    
    // Create floor
    objects.push_back(new Floor({0.0f, 0.0f, 0.0f}, {50.0f, 50.0f}, {50, 0, 12, 255}, physics));
    
    // Create ceiling
    objects.push_back(new Ceiling({0.0f, 5.0f, 0.0f}, {50.0f, 50.0f}, BLACK, physics));
    
    // Create walls (20x20 room)
    objects.push_back(new Wall({0.0f, 2.5f, 10.0f}, {20.0f, 5.0f, 0.5f}, physics));   // North
    objects.push_back(new Wall({0.0f, 2.5f, -10.0f}, {20.0f, 5.0f, 0.5f}, physics));  // South
    objects.push_back(new Wall({10.0f, 2.5f, 0.0f}, {0.5f, 5.0f, 20.0f}, physics));   // East
    objects.push_back(new Wall({-10.0f, 2.5f, 0.0f}, {0.5f, 5.0f, 20.0f}, physics));  // West
    
    // Create light bulb
    objects.push_back(new LightBulb({0.0f, 4.0f, 0.0f}, (Color){120, 140, 200, 255}));
    
    // Create poker table
    PokerTable* pokerTable = new PokerTable({5.0f, 1.0f, 0.0f}, {4.0f, 0.2f, 2.5f}, BROWN, physics);
    objects.push_back(pokerTable);
    
    // Create 3 enemies
    Enemy* enemy1 = new Enemy({-5.0f, 0.0f, 5.0f}, "Person 1");
    Inventory* enemy1Inv = enemy1->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        enemy1Inv->AddItem(chip);
    }
    objects.push_back(enemy1);
    
    Enemy* enemy2 = new Enemy({5.0f, 0.0f, -5.0f}, "Person 2");
    Inventory* enemy2Inv = enemy2->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        enemy2Inv->AddItem(chip);
    }
    objects.push_back(enemy2);
    
    Enemy* enemy3 = new Enemy({-3.0f, 0.0f, -7.0f}, "Person 3");
    Inventory* enemy3Inv = enemy3->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0}, nullptr);
        enemy3Inv->AddItem(chip);
    }
    objects.push_back(enemy3);
    
    // Seat enemies at poker table
    int seat1 = pokerTable->FindClosestOpenSeat(enemy1->position);
    if (seat1 != -1) pokerTable->SeatPerson(enemy1, seat1);
    
    int seat2 = pokerTable->FindClosestOpenSeat(enemy2->position);
    if (seat2 != -1) pokerTable->SeatPerson(enemy2, seat2);
    
    int seat3 = pokerTable->FindClosestOpenSeat(enemy3->position);
    if (seat3 != -1) pokerTable->SeatPerson(enemy3, seat3);
    
    // Create spawners and spawn objects
    Spawner cardSpawner({0.0f, 2.0f, 3.0f}, 2.0f);
    Spawner chipSpawner({-5.0f, 2.0f, -3.0f}, 1.5f);
    Spawner pistolSpawner({3.0f, 2.0f, -5.0f}, 1.0f);
    
    // Note: Spawner adds objects directly to a temporary vector
    // We need to get those objects and add to our objects list
    DOM tempDom;  // Temporary DOM for spawners
    cardSpawner.SpawnCards(SUIT_SPADES, RANK_ACE, 3, physics, &tempDom);
    cardSpawner.SpawnCards(SUIT_HEARTS, RANK_KING, 2, physics, &tempDom);
    chipSpawner.SpawnChips(1, 5, physics, &tempDom);
    chipSpawner.SpawnChips(5, 5, physics, &tempDom);
    chipSpawner.SpawnChips(10, 5, physics, &tempDom);
    chipSpawner.SpawnChips(25, 5, physics, &tempDom);
    chipSpawner.SpawnChips(100, 5, physics, &tempDom);
    pistolSpawner.SpawnPistols(1, physics, &tempDom);
    
    // Move spawned objects from temp DOM to our objects list
    for (int i = 0; i < tempDom.GetCount(); i++) {
        objects.push_back(tempDom.GetObject(i));
    }
    tempDom.Cleanup();  // Clear temp DOM (doesn't delete objects)
    
    return new Scene("game", objects);
}
