#include "catch_amalgamated.hpp"
#include "items/card.hpp"
#include <string>

// Helper to check if type ends with expected suffix
static bool TypeEndsWith(const std::string& type, const std::string& suffix) {
    if (suffix.length() > type.length()) return false;
    return type.compare(type.length() - suffix.length(), suffix.length(), suffix) == 0;
}

TEST_CASE("Card - Construction", "[card]") {
    SECTION("Create card with valid suit and rank") {
        Card card(SUIT_SPADES, RANK_ACE, {0, 0, 0}, nullptr);

        REQUIRE(card.suit == SUIT_SPADES);
        REQUIRE(card.rank == RANK_ACE);
        REQUIRE(card.GetType() == "object_interactable_item_card_spades_ace");
    }

    SECTION("Create card for each suit") {
        Card hearts(SUIT_HEARTS, RANK_KING, {0, 0, 0}, nullptr);
        Card diamonds(SUIT_DIAMONDS, RANK_QUEEN, {0, 0, 0}, nullptr);
        Card clubs(SUIT_CLUBS, RANK_JACK, {0, 0, 0}, nullptr);
        Card spades(SUIT_SPADES, RANK_TEN, {0, 0, 0}, nullptr);

        REQUIRE(hearts.suit == SUIT_HEARTS);
        REQUIRE(diamonds.suit == SUIT_DIAMONDS);
        REQUIRE(clubs.suit == SUIT_CLUBS);
        REQUIRE(spades.suit == SUIT_SPADES);
    }

    SECTION("Create card for each rank") {
        for (int r = RANK_ACE; r <= RANK_KING; r++) {
            Card card(SUIT_HEARTS, static_cast<Rank>(r), {0, 0, 0}, nullptr);
            REQUIRE(card.rank == static_cast<Rank>(r));
        }
    }

    SECTION("Card position is set correctly") {
        Vector3 pos = {5.0f, 10.0f, 15.0f};
        Card card(SUIT_HEARTS, RANK_FIVE, pos, nullptr);

        REQUIRE(card.position.x == 5.0f);
        REQUIRE(card.position.y == 10.0f);
        REQUIRE(card.position.z == 15.0f);
    }
}

TEST_CASE("Card - GetType returns correct string", "[card]") {
    SECTION("Spades suit") {
        REQUIRE(TypeEndsWith(Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr).GetType(), "card_spades_ace"));
        REQUIRE(TypeEndsWith(Card(SUIT_SPADES, RANK_TWO, {0,0,0}, nullptr).GetType(), "card_spades_two"));
        REQUIRE(TypeEndsWith(Card(SUIT_SPADES, RANK_KING, {0,0,0}, nullptr).GetType(), "card_spades_king"));
    }

    SECTION("Hearts suit") {
        REQUIRE(TypeEndsWith(Card(SUIT_HEARTS, RANK_ACE, {0,0,0}, nullptr).GetType(), "card_hearts_ace"));
        REQUIRE(TypeEndsWith(Card(SUIT_HEARTS, RANK_FIVE, {0,0,0}, nullptr).GetType(), "card_hearts_five"));
        REQUIRE(TypeEndsWith(Card(SUIT_HEARTS, RANK_QUEEN, {0,0,0}, nullptr).GetType(), "card_hearts_queen"));
    }

    SECTION("Diamonds suit") {
        REQUIRE(TypeEndsWith(Card(SUIT_DIAMONDS, RANK_ACE, {0,0,0}, nullptr).GetType(), "card_diamonds_ace"));
        REQUIRE(TypeEndsWith(Card(SUIT_DIAMONDS, RANK_JACK, {0,0,0}, nullptr).GetType(), "card_diamonds_jack"));
    }

    SECTION("Clubs suit") {
        REQUIRE(TypeEndsWith(Card(SUIT_CLUBS, RANK_ACE, {0,0,0}, nullptr).GetType(), "card_clubs_ace"));
        REQUIRE(TypeEndsWith(Card(SUIT_CLUBS, RANK_TEN, {0,0,0}, nullptr).GetType(), "card_clubs_ten"));
    }
}

TEST_CASE("Card - Static utility functions", "[card]") {
    SECTION("GetSuitSymbol returns correct symbols") {
        REQUIRE(std::string(Card::GetSuitSymbol(SUIT_HEARTS)) == "HEARTS");
        REQUIRE(std::string(Card::GetSuitSymbol(SUIT_DIAMONDS)) == "DIAMONDS");
        REQUIRE(std::string(Card::GetSuitSymbol(SUIT_CLUBS)) == "CLUBS");
        REQUIRE(std::string(Card::GetSuitSymbol(SUIT_SPADES)) == "SPADES");
    }

    SECTION("GetRankString returns correct strings") {
        REQUIRE(std::string(Card::GetRankString(RANK_ACE)) == "A");
        REQUIRE(std::string(Card::GetRankString(RANK_TWO)) == "2");
        REQUIRE(std::string(Card::GetRankString(RANK_THREE)) == "3");
        REQUIRE(std::string(Card::GetRankString(RANK_TEN)) == "10");
        REQUIRE(std::string(Card::GetRankString(RANK_JACK)) == "J");
        REQUIRE(std::string(Card::GetRankString(RANK_QUEEN)) == "Q");
        REQUIRE(std::string(Card::GetRankString(RANK_KING)) == "K");
    }

    SECTION("GetSuitColor returns correct colors") {
        Color red = Card::GetSuitColor(SUIT_HEARTS);
        Color black = Card::GetSuitColor(SUIT_SPADES);

        REQUIRE(red.r == RED.r);
        REQUIRE(red.g == RED.g);
        REQUIRE(red.b == RED.b);

        REQUIRE(black.r == BLACK.r);
        REQUIRE(black.g == BLACK.g);
        REQUIRE(black.b == BLACK.b);
    }
}

TEST_CASE("Card - Edge Cases", "[card]") {
    SECTION("Multiple cards with same suit/rank are independent") {
        Card card1(SUIT_SPADES, RANK_ACE, {0, 0, 0}, nullptr);
        Card card2(SUIT_SPADES, RANK_ACE, {5, 5, 5}, nullptr);

        REQUIRE(card1.GetType() == card2.GetType());
        REQUIRE(card1.position.x != card2.position.x);
        REQUIRE(&card1 != &card2);
    }

    SECTION("Card canInteract flag defaults to true") {
        Card card(SUIT_HEARTS, RANK_FIVE, {0, 0, 0}, nullptr);
        REQUIRE(card.canInteract == true);
    }

    SECTION("Setting canInteract to false") {
        Card card(SUIT_HEARTS, RANK_FIVE, {0, 0, 0}, nullptr);
        card.canInteract = false;
        REQUIRE(card.canInteract == false);
    }
}
