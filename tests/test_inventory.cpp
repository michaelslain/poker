#include "catch_amalgamated.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}
#include "../include/inventory.hpp"
#include "../include/card.hpp"
#include "../include/chip.hpp"
#include <string>

TEST_CASE("Inventory - Construction", "[inventory]") {
    SECTION("New inventory is empty") {
        Inventory inv;
        REQUIRE(inv.GetStackCount() == 0);
    }
}

TEST_CASE("Inventory - AddItem with Cards", "[inventory]") {
    SECTION("Adding a card creates one stack") {
        Inventory inv;
        Card* card = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        
        inv.AddItem(card);
        
        REQUIRE(inv.GetStackCount() == 1);
        
        delete card;
    }
    
    SECTION("Cards DON'T stack - each card is unique") {
        Inventory inv;
        Card* card1 = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Card* card2 = new Card(SUIT_SPADES, RANK_ACE, {5,5,5}, nullptr);
        
        inv.AddItem(card1);
        inv.AddItem(card2);
        
        // Should be 2 separate stacks, NOT 1 stack with count=2
        REQUIRE(inv.GetStackCount() == 2);
        
        ItemStack* stack1 = inv.GetStack(0);
        ItemStack* stack2 = inv.GetStack(1);
        
        REQUIRE(stack1->count == 1);
        REQUIRE(stack2->count == 1);
        REQUIRE(stack1->item != stack2->item);
        
        delete card1;
        delete card2;
    }
    
    SECTION("Different cards create separate stacks") {
        Inventory inv;
        Card* aceSpades = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Card* kingHearts = new Card(SUIT_HEARTS, RANK_KING, {0,0,0}, nullptr);
        
        inv.AddItem(aceSpades);
        inv.AddItem(kingHearts);
        
        REQUIRE(inv.GetStackCount() == 2);
        
        delete aceSpades;
        delete kingHearts;
    }
    
    SECTION("Adding 5 of the same card creates 5 stacks") {
        Inventory inv;
        Card* cards[5];
        
        for (int i = 0; i < 5; i++) {
            cards[i] = new Card(SUIT_HEARTS, RANK_ACE, {(float)i, 0, 0}, nullptr);
            inv.AddItem(cards[i]);
        }
        
        REQUIRE(inv.GetStackCount() == 5);
        
        for (int i = 0; i < 5; i++) {
            REQUIRE(inv.GetStack(i)->count == 1);
            delete cards[i];
        }
    }
}

TEST_CASE("Inventory - AddItem with Chips", "[inventory]") {
    SECTION("Adding a chip creates one stack") {
        Inventory inv;
        Chip* chip = new Chip(5, {0,0,0}, nullptr);
        
        inv.AddItem(chip);
        
        REQUIRE(inv.GetStackCount() == 1);
        REQUIRE(inv.GetStack(0)->count == 1);
        
        delete chip;
    }
    
    SECTION("Chips of same value DO stack") {
        Inventory inv;
        Chip* chip1 = new Chip(5, {0,0,0}, nullptr);
        Chip* chip2 = new Chip(5, {5,5,5}, nullptr);
        
        inv.AddItem(chip1);
        inv.AddItem(chip2);
        
        // Should be 1 stack with count=2
        REQUIRE(inv.GetStackCount() == 1);
        REQUIRE(inv.GetStack(0)->count == 2);
        
        delete chip1;
        delete chip2;
    }
    
    SECTION("Chips of different values create separate stacks") {
        Inventory inv;
        Chip* chip5 = new Chip(5, {0,0,0}, nullptr);
        Chip* chip10 = new Chip(10, {0,0,0}, nullptr);
        
        inv.AddItem(chip5);
        inv.AddItem(chip10);
        
        REQUIRE(inv.GetStackCount() == 2);
        
        delete chip5;
        delete chip10;
    }
    
    SECTION("Adding 10 chips of value 5 creates 1 stack with count=10") {
        Inventory inv;
        Chip* chips[10];
        
        for (int i = 0; i < 10; i++) {
            chips[i] = new Chip(5, {(float)i, 0, 0}, nullptr);
            inv.AddItem(chips[i]);
        }
        
        REQUIRE(inv.GetStackCount() == 1);
        REQUIRE(inv.GetStack(0)->count == 10);
        
        for (int i = 0; i < 10; i++) {
            delete chips[i];
        }
    }
}

TEST_CASE("Inventory - Mixed Items", "[inventory]") {
    SECTION("Cards and chips create separate stacks") {
        Inventory inv;
        Card* card = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Chip* chip = new Chip(5, {0,0,0}, nullptr);
        
        inv.AddItem(card);
        inv.AddItem(chip);
        
        REQUIRE(inv.GetStackCount() == 2);
        
        delete card;
        delete chip;
    }
    
    SECTION("Multiple cards and multiple chips") {
        Inventory inv;
        
        Card* card1 = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Card* card2 = new Card(SUIT_HEARTS, RANK_KING, {0,0,0}, nullptr);
        Chip* chip1 = new Chip(5, {0,0,0}, nullptr);
        Chip* chip2 = new Chip(5, {1,1,1}, nullptr);
        Chip* chip3 = new Chip(10, {2,2,2}, nullptr);
        
        inv.AddItem(card1);
        inv.AddItem(card2);
        inv.AddItem(chip1);
        inv.AddItem(chip2);
        inv.AddItem(chip3);
        
        // 2 cards + 1 chip stack (2x 5-value) + 1 chip stack (1x 10-value) = 4 stacks
        REQUIRE(inv.GetStackCount() == 4);
        
        delete card1;
        delete card2;
        delete chip1;
        delete chip2;
        delete chip3;
    }
}

TEST_CASE("Inventory - RemoveItem", "[inventory]") {
    SECTION("Removing item decrements count") {
        Inventory inv;
        Chip* chip1 = new Chip(5, {0,0,0}, nullptr);
        Chip* chip2 = new Chip(5, {1,1,1}, nullptr);
        
        inv.AddItem(chip1);
        inv.AddItem(chip2);
        
        REQUIRE(inv.GetStack(0)->count == 2);
        
        inv.RemoveItem(0);
        
        REQUIRE(inv.GetStack(0)->count == 1);
        
        delete chip1;
        delete chip2;
    }
    
    SECTION("Removing last item removes stack") {
        Inventory inv;
        Chip* chip = new Chip(5, {0,0,0}, nullptr);
        
        inv.AddItem(chip);
        REQUIRE(inv.GetStackCount() == 1);
        
        inv.RemoveItem(0);
        REQUIRE(inv.GetStackCount() == 0);
        
        delete chip;
    }
    
    SECTION("Removing invalid index returns false") {
        Inventory inv;
        
        REQUIRE(inv.RemoveItem(0) == false);
        REQUIRE(inv.RemoveItem(-1) == false);
        REQUIRE(inv.RemoveItem(999) == false);
    }
    
    SECTION("Removing from middle preserves other stacks") {
        Inventory inv;
        Card* card1 = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Card* card2 = new Card(SUIT_HEARTS, RANK_KING, {0,0,0}, nullptr);
        Card* card3 = new Card(SUIT_DIAMONDS, RANK_QUEEN, {0,0,0}, nullptr);
        
        inv.AddItem(card1);
        inv.AddItem(card2);
        inv.AddItem(card3);
        
        REQUIRE(inv.GetStackCount() == 3);
        
        inv.RemoveItem(1);  // Remove middle card
        
        REQUIRE(inv.GetStackCount() == 2);
        
        delete card1;
        delete card2;
        delete card3;
    }
}

TEST_CASE("Inventory - GetStack", "[inventory]") {
    SECTION("GetStack returns valid pointer") {
        Inventory inv;
        Card* card = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        
        inv.AddItem(card);
        
        ItemStack* stack = inv.GetStack(0);
        REQUIRE(stack != nullptr);
        REQUIRE(stack->item == card);
        REQUIRE(stack->count == 1);
        
        delete card;
    }
    
    SECTION("GetStack with invalid index returns nullptr") {
        Inventory inv;
        
        REQUIRE(inv.GetStack(0) == nullptr);
        REQUIRE(inv.GetStack(-1) == nullptr);
        REQUIRE(inv.GetStack(999) == nullptr);
    }
    
    SECTION("GetStack returns correct typeString") {
        Inventory inv;
        Card* card = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        
        inv.AddItem(card);
        
        ItemStack* stack = inv.GetStack(0);
        REQUIRE(stack->typeString.find("card_spades_ace") != std::string::npos);
        
        delete card;
    }
}

TEST_CASE("Inventory - Edge Cases", "[inventory]") {
    SECTION("Adding nullptr doesn't crash") {
        Inventory inv;
        
        REQUIRE(inv.AddItem(nullptr) == false);
        REQUIRE(inv.GetStackCount() == 0);
    }
    
    SECTION("Empty inventory cleanup doesn't crash") {
        Inventory inv;
        REQUIRE_NOTHROW(inv.Cleanup());
    }
    
    SECTION("Cleanup removes all stacks") {
        Inventory inv;
        Card* card1 = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Card* card2 = new Card(SUIT_HEARTS, RANK_KING, {0,0,0}, nullptr);
        
        inv.AddItem(card1);
        inv.AddItem(card2);
        
        REQUIRE(inv.GetStackCount() == 2);
        
        inv.Cleanup();
        
        REQUIRE(inv.GetStackCount() == 0);
        
        delete card1;
        delete card2;
    }
    
    SECTION("Large inventory (100 different cards)") {
        Inventory inv;
        Card* cards[100];
        
        for (int i = 0; i < 100; i++) {
            Suit suit = static_cast<Suit>(i % 4);
            Rank rank = static_cast<Rank>((i % 13) + 1);
            cards[i] = new Card(suit, rank, {(float)i, 0, 0}, nullptr);
            inv.AddItem(cards[i]);
        }
        
        REQUIRE(inv.GetStackCount() == 100);
        
        for (int i = 0; i < 100; i++) {
            delete cards[i];
        }
    }
}

TEST_CASE("Inventory - Bug Regression Tests", "[inventory][regression]") {
    SECTION("BUG: Duplicate cards should NOT stack (your reported bug)") {
        Inventory inv;
        
        // Create 2 ace of spades (exactly like you experienced)
        Card* ace1 = new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr);
        Card* ace2 = new Card(SUIT_SPADES, RANK_ACE, {5,5,5}, nullptr);
        
        inv.AddItem(ace1);
        inv.AddItem(ace2);
        
        // CRITICAL: Should be 2 stacks, NOT 1 stack with count=2
        REQUIRE(inv.GetStackCount() == 2);
        
        // Each stack should have count=1
        REQUIRE(inv.GetStack(0)->count == 1);
        REQUIRE(inv.GetStack(1)->count == 1);
        
        // Each stack should have a different card pointer
        REQUIRE(inv.GetStack(0)->item != inv.GetStack(1)->item);
        
        delete ace1;
        delete ace2;
    }
}
