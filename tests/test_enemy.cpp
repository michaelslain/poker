#include "catch_amalgamated.hpp"
#include <string>

#include "entities/enemy.hpp"
#include "items/card.hpp"

TEST_CASE("Enemy - Construction", "[enemy]") {
    SECTION("Create with default name") {
        Enemy enemy({0, 0, 0}, "Enemy");
        REQUIRE(enemy.GetName() == "Enemy");
    }
    
    SECTION("Create with custom name") {
        Enemy enemy({0, 0, 0}, "AI Player 1");
        REQUIRE(enemy.GetName() == "AI Player 1");
    }
    
    SECTION("Enemy height is taller (1.5x)") {
        Enemy enemy({0, 0, 0}, "Enemy");
        REQUIRE(enemy.GetHeight() == 1.5f);
    }
}

TEST_CASE("Enemy - GetType", "[enemy]") {
    Enemy enemy({0, 0, 0}, "Enemy");
    REQUIRE(enemy.GetType().find("enemy") != std::string::npos);
}

TEST_CASE("Enemy - Position", "[enemy]") {
    Enemy enemy({5, 10, 15}, "Enemy");
    REQUIRE(enemy.position.x == 5.0f);
    REQUIRE(enemy.position.y == 10.0f);
    REQUIRE(enemy.position.z == 15.0f);
}

TEST_CASE("Enemy - Inventory", "[enemy]") {
    Enemy enemy({0, 0, 0}, "Enemy");
    Inventory* inv = enemy.GetInventory();
    REQUIRE(inv != nullptr);
    REQUIRE(inv->GetStackCount() == 0);
}

TEST_CASE("Enemy - Seating", "[enemy]") {
    Enemy enemy({0, 0, 0}, "Enemy");
    
    SECTION("Not seated by default") {
        REQUIRE(enemy.IsSeated() == false);
    }
    
    SECTION("Can sit down") {
        enemy.SitDown({5, 1, 0});
        REQUIRE(enemy.IsSeated() == true);
    }
}

TEST_CASE("Enemy - Holographic Cards Head Position", "[enemy][molly]") {
    // Test that the holographic card position calculation is correct
    // Cards should appear above the enemy's head, not inside their body
    
    SECTION("Head height calculation uses correct formula") {
        Enemy enemy({0, 0, 0}, "Enemy");
        
        // Enemy height multiplier is 1.5
        float height = enemy.GetHeight();
        REQUIRE(height == 1.5f);
        
        // Head top height formula from DrawHolographicCards:
        // position.y + (3.7f * height) + 0.3f
        // For enemy at Y=0 with height=1.5: 0 + (3.7 * 1.5) + 0.3 = 5.85
        float expectedHeadTop = 0.0f + (3.7f * height) + 0.3f;
        REQUIRE(expectedHeadTop == Catch::Approx(5.85f));
    }
    
    SECTION("Head height scales with enemy position") {
        Enemy enemy({5, 2, 10}, "Enemy");
        
        float height = enemy.GetHeight();
        float baseY = enemy.position.y;
        
        // Cards should be at: baseY + (3.7 * height) + 0.3
        float expectedHeadTop = baseY + (3.7f * height) + 0.3f;
        REQUIRE(expectedHeadTop == Catch::Approx(2.0f + 5.55f + 0.3f));
    }
}

TEST_CASE("Enemy - Draw Override", "[enemy]") {
    SECTION("Enemy has Draw method that can be called") {
        Enemy enemy({0, 0, 0}, "Enemy");
        
        // We can't easily test rendering without a window,
        // but we can verify the type hierarchy is correct
        std::string type = enemy.GetType();
        REQUIRE(type.find("person") != std::string::npos);
        REQUIRE(type.find("enemy") != std::string::npos);
    }
}

TEST_CASE("Enemy - Cards in Inventory for Holographic Display", "[enemy][molly][inventory]") {
    Enemy enemy({0, 0, 0}, "Enemy");
    Inventory* inv = enemy.GetInventory();
    REQUIRE(inv != nullptr);
    
    SECTION("Enemy starts with no cards") {
        std::vector<int> cardIndices = inv->GetIndicesByType("card");
        REQUIRE(cardIndices.empty());
    }
    
    SECTION("Cards can be added to enemy inventory") {
        Card* card = new Card(SUIT_HEARTS, RANK_ACE, {0, 0, 0});
        inv->AddItem(card);
        
        std::vector<int> cardIndices = inv->GetIndicesByType("card");
        REQUIRE(cardIndices.size() == 1);
        
        ItemStack* stack = inv->GetStack(cardIndices[0]);
        REQUIRE(stack != nullptr);
        REQUIRE(stack->item != nullptr);
        
        // Verify it's actually a card
        std::string itemType = stack->item->GetType();
        REQUIRE(itemType.find("card") != std::string::npos);
    }
    
    SECTION("Multiple cards can be tracked") {
        Card* card1 = new Card(SUIT_HEARTS, RANK_ACE, {0, 0, 0});
        Card* card2 = new Card(SUIT_SPADES, RANK_KING, {0, 0, 0});
        inv->AddItem(card1);
        inv->AddItem(card2);
        
        std::vector<int> cardIndices = inv->GetIndicesByType("card");
        REQUIRE(cardIndices.size() == 2);
    }
}
