#include "scenes/game_scene.hpp"
#include "core/scene.hpp"
#include "entities/player.hpp"
#include "entities/enemy.hpp"
#include "world/floor.hpp"
#include "world/ceiling.hpp"
#include "world/wall.hpp"
#include "rendering/light_bulb.hpp"
#include "gameplay/poker_table.hpp"
#include "world/spawner.hpp"
#include "items/chip.hpp"
#include "items/card.hpp"
#include "weapons/pistol.hpp"
#include "substances/adrenaline.hpp"
#include "substances/salvia.hpp"
#include "substances/cocaine.hpp"
#include "substances/shrooms.hpp"
#include "substances/vodka.hpp"
#include "substances/weed.hpp"
#include "substances/molly.hpp"

Scene* CreateGameScene(PhysicsWorld* physics) {
    std::vector<Object*> objects;

    // Set global physics instance so spawners can access it
    PhysicsWorld::SetGlobal(physics);

    // Create player
    Player* player = new Player({0.0f, 0.0f, 0.0f}, "Player");

    // Give player starting chips (500 chips)
    Inventory* playerInv = player->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0});
        playerInv->AddItem(chip);
    }
    objects.push_back(player);

    // Create floor
    objects.push_back(new Floor({0.0f, 0.0f, 0.0f}, {50.0f, 50.0f}, {50, 0, 12, 255}));

    // Create ceiling
    objects.push_back(new Ceiling({0.0f, 5.0f, 0.0f}, {50.0f, 50.0f}, BLACK));

    // Create walls (20x20 room)
    objects.push_back(new Wall({0.0f, 2.5f, 10.0f}, {20.0f, 5.0f, 0.5f}));   // North
    objects.push_back(new Wall({0.0f, 2.5f, -10.0f}, {20.0f, 5.0f, 0.5f}));  // South
    objects.push_back(new Wall({10.0f, 2.5f, 0.0f}, {0.5f, 5.0f, 20.0f}));   // East
    objects.push_back(new Wall({-10.0f, 2.5f, 0.0f}, {0.5f, 5.0f, 20.0f}));  // West

    // Create light bulb
    objects.push_back(new LightBulb({0.0f, 4.0f, 0.0f}, (Color){120, 140, 200, 255}));

    // Create poker table
    PokerTable* pokerTable = new PokerTable({5.0f, 1.0f, 0.0f}, {4.0f, 0.2f, 2.5f}, BROWN);
    objects.push_back(pokerTable);

    // Create 3 enemies
    Enemy* enemy1 = new Enemy({-5.0f, 0.0f, 5.0f}, "Person 1");
    Inventory* enemy1Inv = enemy1->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0});
        enemy1Inv->AddItem(chip);
    }
    objects.push_back(enemy1);

    Enemy* enemy2 = new Enemy({5.0f, 0.0f, -5.0f}, "Person 2");
    Inventory* enemy2Inv = enemy2->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0});
        enemy2Inv->AddItem(chip);
    }
    objects.push_back(enemy2);

    Enemy* enemy3 = new Enemy({-3.0f, 0.0f, -7.0f}, "Person 3");
    Inventory* enemy3Inv = enemy3->GetInventory();
    for (int i = 0; i < 5; i++) {
        Chip* chip = new Chip(100, {0, 0, 0});
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

    // Create spawners - they spawn automatically on construction and add to global DOM
    // Spawners take ownership of the template object and delete it in destructor

    // Card spawners
    objects.push_back(new Spawner({0.0f, 2.0f, 3.0f}, 2.0f, new Card(SUIT_SPADES, RANK_ACE, {0, 0, 0}), 3));
    objects.push_back(new Spawner({0.0f, 2.0f, 3.0f}, 2.0f, new Card(SUIT_HEARTS, RANK_KING, {0, 0, 0}), 2));

    // Chip spawners
    objects.push_back(new Spawner({-5.0f, 2.0f, -3.0f}, 1.5f, new Chip(1, {0, 0, 0}), 5));
    objects.push_back(new Spawner({-5.0f, 2.0f, -3.0f}, 1.5f, new Chip(5, {0, 0, 0}), 5));
    objects.push_back(new Spawner({-5.0f, 2.0f, -3.0f}, 1.5f, new Chip(10, {0, 0, 0}), 5));
    objects.push_back(new Spawner({-5.0f, 2.0f, -3.0f}, 1.5f, new Chip(25, {0, 0, 0}), 5));
    objects.push_back(new Spawner({-5.0f, 2.0f, -3.0f}, 1.5f, new Chip(100, {0, 0, 0}), 5));

    // Weapon spawner
    objects.push_back(new Spawner({3.0f, 2.0f, -5.0f}, 1.0f, new Pistol({0, 0, 0}), 1));

    // Substance spawners (scattered around the room)
    objects.push_back(new Spawner({7.0f, 2.0f, 5.0f}, 1.0f, new Adrenaline({0, 0, 0}), 2));    // Red - near east wall
    objects.push_back(new Spawner({-7.0f, 2.0f, 7.0f}, 0.8f, new Salvia({0, 0, 0}), 2));       // Green - northwest corner
    objects.push_back(new Spawner({7.0f, 2.0f, -7.0f}, 1.0f, new Cocaine({0, 0, 0}), 3));      // White - southeast corner
    objects.push_back(new Spawner({-7.0f, 2.0f, -5.0f}, 0.8f, new Shrooms({0, 0, 0}), 2));     // Purple - southwest area
    objects.push_back(new Spawner({0.0f, 2.0f, -7.0f}, 1.2f, new Vodka({0, 0, 0}), 3));        // Clear - south wall
    objects.push_back(new Spawner({-3.0f, 2.0f, 5.0f}, 0.8f, new Weed({0, 0, 0}), 2));         // Dark green - north area
    objects.push_back(new Spawner({6.0f, 2.0f, 0.0f}, 0.8f, new Molly({0, 0, 0}), 2));         // Pink - east side

    return new Scene("game", objects);
}
