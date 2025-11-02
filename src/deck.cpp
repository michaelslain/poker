#include "deck.hpp"
#include <cstdlib>
#include <ctime>

Deck::Deck() : count(0) {
    // Generate all 52 cards
    int index = 0;
    for (int suit = SUIT_HEARTS; suit <= SUIT_SPADES; suit++) {
        for (int rank = RANK_ACE; rank <= RANK_KING; rank++) {
            // Create card at origin with no physics
            cards[index++] = new Card(static_cast<Suit>(suit), static_cast<Rank>(rank), {0, 0, 0}, nullptr);
        }
    }
    
    count = DECK_SIZE;
}

Deck::~Deck() {
    Cleanup();
}

void Deck::Shuffle() {
    if (count <= 1) return;
    
    // Fisher-Yates shuffle algorithm
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap cards[i] and cards[j]
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }
}

Card* Deck::Draw() {
    if (count <= 0) {
        return nullptr;
    }
    
    // Draw from the "top" (end of array)
    count--;
    return cards[count];
}

Card* Deck::Peek() {
    if (count <= 0) {
        return nullptr;
    }
    
    return cards[count - 1];
}

void Deck::Reset() {
    // Reset count to full deck (doesn't recreate cards)
    count = DECK_SIZE;
}

void Deck::Cleanup() {
    // Cleanup and free all cards
    for (int i = 0; i < DECK_SIZE; i++) {
        if (cards[i]) {
            delete cards[i];
            cards[i] = nullptr;
        }
    }
    count = 0;
}
