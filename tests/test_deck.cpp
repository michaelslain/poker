#include "catch_amalgamated.hpp"
#include "../include/deck.hpp"
#include <set>
#include <string>

TEST_CASE("Deck - Construction", "[deck]") {
    SECTION("Deck creates 52 cards") {
        Deck deck({0, 0, 0});
        REQUIRE(deck.GetCount() == 52);
    }
    
    SECTION("Deck is not empty after construction") {
        Deck deck({0, 0, 0});
        REQUIRE_FALSE(deck.IsEmpty());
    }
    
    SECTION("Deck contains all unique cards") {
        Deck deck({0, 0, 0});
        std::set<std::string> uniqueCards;
        
        // Draw all cards and check uniqueness
        for (int i = 0; i < 52; i++) {
            Card* card = deck.DrawCard();
            REQUIRE(card != nullptr);
            uniqueCards.insert(card->GetType());
        }
        
        REQUIRE(uniqueCards.size() == 52);
        REQUIRE(deck.IsEmpty());
    }
    
    SECTION("Deck contains all suits") {
        Deck deck({0, 0, 0});
        std::set<Suit> suits;
        
        for (int i = 0; i < 52; i++) {
            Card* card = deck.DrawCard();
            suits.insert(card->suit);
        }
        
        REQUIRE(suits.size() == 4);
        REQUIRE(suits.count(SUIT_HEARTS) == 1);
        REQUIRE(suits.count(SUIT_DIAMONDS) == 1);
        REQUIRE(suits.count(SUIT_CLUBS) == 1);
        REQUIRE(suits.count(SUIT_SPADES) == 1);
    }
    
    SECTION("Deck contains all ranks") {
        Deck deck({0, 0, 0});
        std::set<Rank> ranks;
        
        for (int i = 0; i < 52; i++) {
            Card* card = deck.DrawCard();
            ranks.insert(card->rank);
        }
        
        REQUIRE(ranks.size() == 13);
        for (int r = RANK_ACE; r <= RANK_KING; r++) {
            REQUIRE(ranks.count(static_cast<Rank>(r)) == 1);
        }
    }
}

TEST_CASE("Deck - DrawCard", "[deck]") {
    SECTION("DrawCard returns valid card") {
        Deck deck({0, 0, 0});
        Card* card = deck.DrawCard();
        
        REQUIRE(card != nullptr);
        REQUIRE(deck.GetCount() == 51);
    }
    
    SECTION("DrawCard decrements count") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 10; i++) {
            deck.DrawCard();
        }
        
        REQUIRE(deck.GetCount() == 42);
    }
    
    SECTION("DrawCard returns nullptr when empty") {
        Deck deck({0, 0, 0});
        
        // Draw all 52 cards
        for (int i = 0; i < 52; i++) {
            deck.DrawCard();
        }
        
        REQUIRE(deck.IsEmpty());
        REQUIRE(deck.DrawCard() == nullptr);
    }
    
    SECTION("Multiple DrawCard calls return different cards") {
        Deck deck({0, 0, 0});
        Card* card1 = deck.DrawCard();
        Card* card2 = deck.DrawCard();
        
        REQUIRE(card1 != card2);
    }
}

TEST_CASE("Deck - Peek", "[deck]") {
    SECTION("Peek returns top card without removing it") {
        Deck deck({0, 0, 0});
        Card* peeked = deck.Peek();
        
        REQUIRE(peeked != nullptr);
        REQUIRE(deck.GetCount() == 52);
    }
    
    SECTION("Peek returns same card multiple times") {
        Deck deck({0, 0, 0});
        Card* peek1 = deck.Peek();
        Card* peek2 = deck.Peek();
        
        REQUIRE(peek1 == peek2);
    }
    
    SECTION("Peek and DrawCard return same card") {
        Deck deck({0, 0, 0});
        Card* peeked = deck.Peek();
        Card* drawn = deck.DrawCard();
        
        REQUIRE(peeked == drawn);
        REQUIRE(deck.GetCount() == 51);
    }
    
    SECTION("Peek on empty deck returns nullptr") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 52; i++) {
            deck.DrawCard();
        }
        
        REQUIRE(deck.Peek() == nullptr);
    }
}

TEST_CASE("Deck - Shuffle", "[deck]") {
    SECTION("Shuffle doesn't change deck count") {
        Deck deck({0, 0, 0});
        deck.Shuffle();
        
        REQUIRE(deck.GetCount() == 52);
    }
    
    SECTION("Shuffle changes card order") {
        Deck deck1({0, 0, 0});
        Deck deck2({0, 0, 0});
        
        deck2.Shuffle();
        
        // Draw a few cards and compare
        bool orderChanged = false;
        for (int i = 0; i < 10; i++) {
            if (deck1.DrawCard() != deck2.DrawCard()) {
                orderChanged = true;
                break;
            }
        }
        
        REQUIRE(orderChanged);
    }
    
    SECTION("Shuffle on empty deck doesn't crash") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 52; i++) {
            deck.DrawCard();
        }
        
        REQUIRE_NOTHROW(deck.Shuffle());
    }
    
    SECTION("Shuffle with one card doesn't crash") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 51; i++) {
            deck.DrawCard();
        }
        
        REQUIRE_NOTHROW(deck.Shuffle());
        REQUIRE(deck.GetCount() == 1);
    }
}

TEST_CASE("Deck - Reset", "[deck]") {
    SECTION("Reset restores all 52 cards") {
        Deck deck({0, 0, 0});
        
        // Draw some cards
        for (int i = 0; i < 20; i++) {
            deck.DrawCard();
        }
        
        deck.Reset();
        REQUIRE(deck.GetCount() == 52);
    }
    
    SECTION("Reset after drawing all cards") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 52; i++) {
            deck.DrawCard();
        }
        
        REQUIRE(deck.IsEmpty());
        
        deck.Reset();
        REQUIRE(deck.GetCount() == 52);
        REQUIRE_FALSE(deck.IsEmpty());
    }
    
    SECTION("Reset on full deck doesn't break anything") {
        Deck deck({0, 0, 0});
        deck.Reset();
        
        REQUIRE(deck.GetCount() == 52);
    }
    
    SECTION("Cards can be drawn after reset") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 52; i++) {
            deck.DrawCard();
        }
        
        deck.Reset();
        
        Card* card = deck.DrawCard();
        REQUIRE(card != nullptr);
        REQUIRE(deck.GetCount() == 51);
    }
}

TEST_CASE("Deck - Edge Cases", "[deck]") {
    SECTION("Drawing all 52 cards one by one") {
        Deck deck({0, 0, 0});
        
        for (int i = 0; i < 52; i++) {
            REQUIRE(deck.GetCount() == 52 - i);
            Card* card = deck.DrawCard();
            REQUIRE(card != nullptr);
        }
        
        REQUIRE(deck.IsEmpty());
        REQUIRE(deck.DrawCard() == nullptr);
    }
    
    SECTION("Multiple reset and draw cycles") {
        Deck deck({0, 0, 0});
        
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int i = 0; i < 52; i++) {
                deck.DrawCard();
            }
            
            REQUIRE(deck.IsEmpty());
            deck.Reset();
            REQUIRE(deck.GetCount() == 52);
        }
    }
    
    SECTION("Shuffle then reset preserves all cards") {
        Deck deck({0, 0, 0});
        deck.Shuffle();
        deck.Reset();
        
        REQUIRE(deck.GetCount() == 52);
        
        // All 52 cards should still be drawable
        for (int i = 0; i < 52; i++) {
            REQUIRE(deck.DrawCard() != nullptr);
        }
    }
}
