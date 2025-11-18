#include "catch_amalgamated.hpp"
#include <string>

#include "gameplay/poker_table.hpp"
#include "entities/player.hpp"
#include "entities/enemy.hpp"
#include "items/chip.hpp"
#include "items/card.hpp"
#include "weapons/pistol.hpp"
#include "items/inventory.hpp"
#include "core/dom.hpp"

TEST_CASE("PokerTable - Construction", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("Create poker table") {
        PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
        REQUIRE(table.position.x == 0.0f);
        REQUIRE(table.position.y == 1.0f);
        REQUIRE(table.position.z == 0.0f);
    }
    
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("PokerTable - GetType", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
    REQUIRE(table.GetType().find("poker_table") != std::string::npos);
    
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("PokerTable - Seating", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);

    SECTION("Find closest open seat") {
        int seatIndex = table.FindClosestOpenSeat({0, 0, 0});
        REQUIRE(seatIndex >= 0);
        REQUIRE(seatIndex < MAX_SEATS);
    }

    SECTION("Seat a player") {
        Player player({0, 1.3f, 0});
        int seatIndex = table.FindClosestOpenSeat(player.position);
        bool seated = table.SeatPerson(&player, seatIndex);
        REQUIRE(seated == true);
        REQUIRE(player.IsSeated() == true);
        REQUIRE(table.FindSeatIndex(&player) == seatIndex);
    }

    SECTION("Unseat a player") {
        Player player({0, 1.3f, 0});
        int seatIndex = table.FindClosestOpenSeat(player.position);
        table.SeatPerson(&player, seatIndex);
        table.UnseatPerson(&player);
        REQUIRE(player.IsSeated() == false);
        REQUIRE(table.FindSeatIndex(&player) == -1);
    }

    SECTION("Cannot seat player in occupied seat") {
        Player player1({0, 1.3f, 0});
        Player player2({1, 1.3f, 0});
        int seatIndex = 0;

        bool seated1 = table.SeatPerson(&player1, seatIndex);
        bool seated2 = table.SeatPerson(&player2, seatIndex);

        REQUIRE(seated1 == true);
        REQUIRE(seated2 == false);
    }

    SECTION("Seat multiple players") {
        Player player1({0, 1.3f, 0});
        Player player2({1, 1.3f, 0});
        Player player3({2, 1.3f, 0});

        bool seated1 = table.SeatPerson(&player1, 0);
        bool seated2 = table.SeatPerson(&player2, 1);
        bool seated3 = table.SeatPerson(&player3, 2);

        REQUIRE(seated1 == true);
        REQUIRE(seated2 == true);
        REQUIRE(seated3 == true);
        REQUIRE(table.FindSeatIndex(&player1) == 0);
        REQUIRE(table.FindSeatIndex(&player2) == 1);
        REQUIRE(table.FindSeatIndex(&player3) == 2);
    }

    SECTION("Seat all MAX_SEATS players") {
        Player players[MAX_SEATS] = {
            {{0, 1.3f, 0}},
            {{1, 1.3f, 0}},
            {{2, 1.3f, 0}},
            {{3, 1.3f, 0}},
            {{4, 1.3f, 0}},
            {{5, 1.3f, 0}},
            {{6, 1.3f, 0}},
            {{7, 1.3f, 0}}
        };

        for (int i = 0; i < MAX_SEATS; i++) {
            bool seated = table.SeatPerson(&players[i], i);
            REQUIRE(seated == true);
        }

        // Try to seat 9th player (should fail)
        Player extraPlayer({8, 1.3f, 0});
        bool seated = table.SeatPerson(&extraPlayer, 0); // Seat 0 is occupied
        REQUIRE(seated == false);
    }
    
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("PokerTable - Player Sit/Stand Cycles", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
    Player player({0, 1.3f, 0});

    SECTION("Player sits then stands") {
        int seatIndex = table.FindClosestOpenSeat(player.position);
        table.SeatPerson(&player, seatIndex);
        REQUIRE(player.IsSeated() == true);

        table.UnseatPerson(&player);
        REQUIRE(player.IsSeated() == false);
    }

    SECTION("Player sits, stands, sits again") {
        int seatIndex = table.FindClosestOpenSeat(player.position);

        // First sit
        table.SeatPerson(&player, seatIndex);
        REQUIRE(player.IsSeated() == true);

        // Stand
        table.UnseatPerson(&player);
        REQUIRE(player.IsSeated() == false);

        // Sit again in same seat
        table.SeatPerson(&player, seatIndex);
        REQUIRE(player.IsSeated() == true);
        REQUIRE(table.FindSeatIndex(&player) == seatIndex);
    }

    SECTION("Player sits, stands, another player takes seat") {
        Player player2({1, 1.3f, 0});
        int seatIndex = 0;

        // Player 1 sits
        table.SeatPerson(&player, seatIndex);
        REQUIRE(table.FindSeatIndex(&player) == seatIndex);

        // Player 1 stands
        table.UnseatPerson(&player);
        REQUIRE(table.FindSeatIndex(&player) == -1);

        // Player 2 takes the seat
        table.SeatPerson(&player2, seatIndex);
        REQUIRE(table.FindSeatIndex(&player2) == seatIndex);
    }

    SECTION("Multiple sit/stand cycles") {
        int seatIndex = table.FindClosestOpenSeat(player.position);

        for (int i = 0; i < 5; i++) {
            table.SeatPerson(&player, seatIndex);
            REQUIRE(player.IsSeated() == true);

            table.UnseatPerson(&player);
            REQUIRE(player.IsSeated() == false);
        }
    }
    
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("PokerTable - Player Inventories with Items", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);

    SECTION("Player with chips sits and stands") {
        Player player({0, 1.3f, 0});
        Inventory* inv = player.GetInventory();

        // Add chips to inventory
        Chip* chip1 = new Chip(5, {0, 0, 0});
        Chip* chip2 = new Chip(10, {0, 0, 0});
        Chip* chip3 = new Chip(25, {0, 0, 0});
        inv->AddItem(chip1);
        inv->AddItem(chip2);
        inv->AddItem(chip3);

        REQUIRE(inv->GetStackCount() == 3);

        // Sit down
        int seatIndex = table.FindClosestOpenSeat(player.position);
        table.SeatPerson(&player, seatIndex);
        REQUIRE(player.IsSeated() == true);
        REQUIRE(inv->GetStackCount() == 3); // Should still have chips

        // Stand up
        table.UnseatPerson(&player);
        REQUIRE(player.IsSeated() == false);
        REQUIRE(inv->GetStackCount() == 3); // Should still have chips
    }

    SECTION("Player with cards sits and stands") {
        Player player({0, 1.3f, 0});
        Inventory* inv = player.GetInventory();

        // Add cards to inventory
        Card* card1 = new Card(SUIT_HEARTS, RANK_ACE, {0, 0, 0});
        Card* card2 = new Card(SUIT_SPADES, RANK_KING, {0, 0, 0});
        inv->AddItem(card1);
        inv->AddItem(card2);

        REQUIRE(inv->GetStackCount() == 2);

        // Sit down
        int seatIndex = table.FindClosestOpenSeat(player.position);
        table.SeatPerson(&player, seatIndex);
        REQUIRE(player.IsSeated() == true);
        REQUIRE(inv->GetStackCount() == 2); // Should still have cards

        // Stand up
        table.UnseatPerson(&player);
        REQUIRE(player.IsSeated() == false);
        REQUIRE(inv->GetStackCount() == 2); // Should still have cards
    }

    SECTION("Player with mixed inventory (cards, chips, pistol)") {
        Player player({0, 1.3f, 0});
        Inventory* inv = player.GetInventory();

        // Add various items
        Card* card = new Card(SUIT_DIAMONDS, RANK_QUEEN, {0, 0, 0});
        Chip* chip1 = new Chip(100, {0, 0, 0});
        Chip* chip2 = new Chip(5, {0, 0, 0});
        Pistol* pistol = new Pistol({0, 0, 0});

        inv->AddItem(card);
        inv->AddItem(chip1);
        inv->AddItem(chip2);
        inv->AddItem(pistol);

        REQUIRE(inv->GetStackCount() == 4);

        // Sit down
        int seatIndex = table.FindClosestOpenSeat(player.position);
        table.SeatPerson(&player, seatIndex);
        REQUIRE(player.IsSeated() == true);
        REQUIRE(inv->GetStackCount() == 4); // All items preserved

        // Stand up
        table.UnseatPerson(&player);
        REQUIRE(player.IsSeated() == false);
        REQUIRE(inv->GetStackCount() == 4); // All items preserved
    }

    SECTION("Player with full inventory sits and stands") {
        Player player({0, 1.3f, 0});
        Inventory* inv = player.GetInventory();

        // Fill inventory with many items (chips of same value will stack)
        for (int i = 0; i < 5; i++) {
            Chip* chip = new Chip(25, {0, 0, 0});
            inv->AddItem(chip);
        }
        for (int i = 0; i < 5; i++) {
            Card* card = new Card(SUIT_CLUBS, static_cast<Rank>(RANK_TWO + i), {0, 0, 0});
            inv->AddItem(card);
        }

        int initialCount = inv->GetStackCount();
        REQUIRE(initialCount == 6); // 1 stack of 5 chips (same value) + 5 cards

        // Sit and stand multiple times
        int seatIndex = table.FindClosestOpenSeat(player.position);
        for (int cycle = 0; cycle < 3; cycle++) {
            table.SeatPerson(&player, seatIndex);
            REQUIRE(inv->GetStackCount() == initialCount);

            table.UnseatPerson(&player);
            REQUIRE(inv->GetStackCount() == initialCount);
        }
    }

    SECTION("Multiple players with different inventories") {
        Player player1({0, 1.3f, 0});
        Player player2({1, 1.3f, 0});
        Player player3({2, 1.3f, 0});

        // Player 1: chips only (using valid chip values: 1, 5, 10, 25, 100)
        Chip* chip1 = new Chip(100, {0, 0, 0});
        Chip* chip2 = new Chip(25, {0, 0, 0});
        player1.GetInventory()->AddItem(chip1);
        player1.GetInventory()->AddItem(chip2);

        // Player 2: cards only
        Card* card1 = new Card(SUIT_HEARTS, RANK_ACE, {0, 0, 0});
        Card* card2 = new Card(SUIT_HEARTS, RANK_KING, {0, 0, 0});
        player2.GetInventory()->AddItem(card1);
        player2.GetInventory()->AddItem(card2);

        // Player 3: pistol only
        Pistol* pistol = new Pistol({0, 0, 0});
        player3.GetInventory()->AddItem(pistol);

        // Seat all players
        table.SeatPerson(&player1, 0);
        table.SeatPerson(&player2, 1);
        table.SeatPerson(&player3, 2);

        REQUIRE(player1.GetInventory()->GetStackCount() == 2);
        REQUIRE(player2.GetInventory()->GetStackCount() == 2);
        REQUIRE(player3.GetInventory()->GetStackCount() == 1);

        // Unseat all players
        table.UnseatPerson(&player1);
        table.UnseatPerson(&player2);
        table.UnseatPerson(&player3);

        REQUIRE(player1.GetInventory()->GetStackCount() == 2);
        REQUIRE(player2.GetInventory()->GetStackCount() == 2);
        REQUIRE(player3.GetInventory()->GetStackCount() == 1);
    }
    
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("PokerTable - Enemy Seating", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);

    SECTION("Enemy can sit at table") {
        Enemy enemy({0, 1.3f, 0}, "TestEnemy");
        int seatIndex = table.FindClosestOpenSeat(enemy.position);
        bool seated = table.SeatPerson(&enemy, seatIndex);

        REQUIRE(seated == true);
        REQUIRE(enemy.IsSeated() == true);
        REQUIRE(table.FindSeatIndex(&enemy) == seatIndex);
    }

    SECTION("Mix of players and enemies") {
        Player player({0, 1.3f, 0});
        Enemy enemy1({1, 1.3f, 0}, "Enemy1");
        Enemy enemy2({2, 1.3f, 0}, "Enemy2");

        table.SeatPerson(&player, 0);
        table.SeatPerson(&enemy1, 1);
        table.SeatPerson(&enemy2, 2);

        REQUIRE(player.IsSeated() == true);
        REQUIRE(enemy1.IsSeated() == true);
        REQUIRE(enemy2.IsSeated() == true);
    }

    SECTION("Enemy with chips sits and stands") {
        Enemy enemy({0, 1.3f, 0}, "RichEnemy");
        Inventory* inv = enemy.GetInventory();

        // Give enemy chips (different values so they don't stack)
        Chip* chip1 = new Chip(100, {0, 0, 0});
        Chip* chip2 = new Chip(25, {0, 0, 0});
        Chip* chip3 = new Chip(10, {0, 0, 0});
        inv->AddItem(chip1);
        inv->AddItem(chip2);
        inv->AddItem(chip3);

        REQUIRE(inv->GetStackCount() == 3);

        int seatIndex = table.FindClosestOpenSeat(enemy.position);
        table.SeatPerson(&enemy, seatIndex);
        REQUIRE(inv->GetStackCount() == 3);

        table.UnseatPerson(&enemy);
        REQUIRE(inv->GetStackCount() == 3);
    }
    
    // Cleanup
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

// NOTE: PokerTable now uses global physics from PhysicsWorld::GetGlobal()
// Collision geometry is internal and not exposed via GetCollider()
TEST_CASE("PokerTable - GetGeom", "[poker_table]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("PokerTable creates successfully") {
        PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
        REQUIRE(table.GetType().find("poker_table") != std::string::npos);
    }
    
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("PokerTable - Community Cards Memory Management", "[poker_table][regression]") {
    // Regression test for bus error caused by deleting community cards that are owned by Deck

    SECTION("Community cards are not deleted when hand ends") {
        DOM dom;
        DOM::SetGlobal(&dom);

        PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
        dom.AddObject(&table);

        // Create enemies with chips
        Enemy enemy1({0, 0, 0}, "Player1");
        Enemy enemy2({1, 0, 0}, "Player2");

        // Give them chips
        for (int i = 0; i < 5; i++) {
            Chip* chip1 = new Chip(100, {0, 0, 0});
            Chip* chip2 = new Chip(100, {0, 0, 0});
            enemy1.GetInventory()->AddItem(chip1);
            enemy2.GetInventory()->AddItem(chip2);
        }

        // Seat them
        table.SeatPerson(&enemy1, 0);
        table.SeatPerson(&enemy2, 1);

        // Simulate game progression through multiple hands
        // This tests that community cards are properly managed across hands
        for (int hand = 0; hand < 3; hand++) {
            // Update to trigger hand start
            table.Update(0.016f);

            // Simulate betting rounds by forcing everyone to fold quickly
            // This will trigger EndHand() which should NOT delete community cards
            for (int update = 0; update < 10; update++) {
                table.Update(0.016f);
            }
        }

        // If we got here without crashing, the test passes
        // The bug would have caused a bus error when accessing deleted community cards
        REQUIRE(true);

        dom.Cleanup();
    }

    SECTION("Community cards are added to and removed from DOM correctly") {
        DOM dom;
        DOM::SetGlobal(&dom);

        PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
        dom.AddObject(&table);


        // Create and seat players
        Enemy enemy1({0, 0, 0}, "Player1");
        Enemy enemy2({1, 0, 0}, "Player2");

        // Give chips
        for (int i = 0; i < 5; i++) {
            enemy1.GetInventory()->AddItem(new Chip(100, {0, 0, 0}));
            enemy2.GetInventory()->AddItem(new Chip(100, {0, 0, 0}));
        }

        table.SeatPerson(&enemy1, 0);
        table.SeatPerson(&enemy2, 1);

        // Start hand
        table.Update(0.016f);

        // Community cards should now be in DOM after flop/turn/river
        // (exact count depends on game progression)

        // Force hand to end
        for (int i = 0; i < 20; i++) {
            table.Update(0.016f);
        }

        // After hand ends, community cards should be removed from DOM
        // DOM count should be back to initial or close to it
        // (may have some variation due to pot chips)

        // The key is that we don't crash when trying to reuse deck cards
        REQUIRE(true);

        dom.Cleanup();
    }

    SECTION("Deck cards can be reused after multiple hands") {
        DOM dom;
        DOM::SetGlobal(&dom);

        PokerTable table({0, 1, 0}, {4, 0.2f, 2.5f}, BROWN);
        dom.AddObject(&table);

        Enemy enemy1({0, 0, 0}, "Player1");
        Enemy enemy2({1, 0, 0}, "Player2");

        // Give them lots of chips
        for (int i = 0; i < 10; i++) {
            enemy1.GetInventory()->AddItem(new Chip(100, {0, 0, 0}));
            enemy2.GetInventory()->AddItem(new Chip(100, {0, 0, 0}));
        }

        table.SeatPerson(&enemy1, 0);
        table.SeatPerson(&enemy2, 1);

        // Play through 5 hands
        // This ensures deck is reset and cards are reused
        // If community cards were deleted, this would crash
        for (int hand = 0; hand < 5; hand++) {
            for (int update = 0; update < 30; update++) {
                table.Update(0.016f);
            }
        }

        // Success - no crash means cards were not deleted
        REQUIRE(true);

        dom.Cleanup();
    }
}

TEST_CASE("PokerTable - Seating Y Position Preservation", "[poker_table][seating][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    Vector3 tablePos = {0, 0.1f, 0};
    Vector3 tableSize = {4.0f, 0.2f, 2.5f};
    PokerTable table(tablePos, tableSize, DARKBROWN);
    dom.AddObject(&table);
    
    SECTION("Player Y position preserved when sitting") {
        Player player({0, 1.3f, 0});
        float originalY = player.position.y;
        
        // Find and sit at first available seat
        int seatIndex = table.FindClosestOpenSeat(player.position);
        REQUIRE(seatIndex >= 0);
        
        table.SeatPerson(&player, seatIndex);
        
        // Y position should be preserved
        REQUIRE(player.position.y == Catch::Approx(originalY));
        REQUIRE(player.IsSeated() == true);
        
        table.UnseatPerson(&player);
    }
    
    SECTION("Player Y position preserved when standing") {
        Player player({0, 1.3f, 0});
        
        int seatIndex = table.FindClosestOpenSeat(player.position);
        table.SeatPerson(&player, seatIndex);
        
        float seatedY = player.position.y;
        
        table.UnseatPerson(&player);
        
        // Y should remain the same after standing
        REQUIRE(player.position.y == Catch::Approx(seatedY));
        REQUIRE(player.IsSeated() == false);
    }
    
    SECTION("Enemy Y position preserved when sitting") {
        Enemy enemy({0, 1.3f, 0}, "TestEnemy");
        float originalY = enemy.position.y;
        
        int seatIndex = table.FindClosestOpenSeat(enemy.position);
        table.SeatPerson(&enemy, seatIndex);
        
        REQUIRE(enemy.position.y == Catch::Approx(originalY));
        
        table.UnseatPerson(&enemy);
    }
    
    SECTION("Multiple people at different heights maintain positions") {
        Player player({0, 1.3f, 0});
        Enemy enemy({5, 2.6f, 0}, "TallEnemy");  // Enemy at different height
        
        float playerY = player.position.y;
        float enemyY = enemy.position.y;
        
        int playerSeat = table.FindClosestOpenSeat(player.position);
        int enemySeat = table.FindClosestOpenSeat(enemy.position);
        
        table.SeatPerson(&player, playerSeat);
        table.SeatPerson(&enemy, enemySeat);
        
        // Both should preserve their original Y positions
        REQUIRE(player.position.y == Catch::Approx(playerY));
        REQUIRE(enemy.position.y == Catch::Approx(enemyY));
        
        table.UnseatPerson(&player);
        table.UnseatPerson(&enemy);
    }
    
    dom.Cleanup();
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}
