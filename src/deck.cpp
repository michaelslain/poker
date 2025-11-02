#include "deck.hpp"
#include <cstdlib>
#include <ctime>

Deck::Deck(Vector3 pos) : Object(pos), count(0) {
    TraceLog(LOG_INFO, "Deck: Creating deck at position (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
    
    // Generate all 52 cards
    // Cards are stored in the cards[] array for game logic
    // AND added as children for automatic recursive rendering
    int index = 0;
    for (int suit = SUIT_HEARTS; suit <= SUIT_SPADES; suit++) {
        for (int rank = RANK_ACE; rank <= RANK_KING; rank++) {
            // Create card at origin with no physics (cards are part of the deck, not individual objects)
            Card* card = new Card(static_cast<Suit>(suit), static_cast<Rank>(rank), {0, 0, 0}, nullptr);
            card->isDynamicallyAllocated = true;
            cards[index] = card;
            
            // Add card as child for rendering
            // Both cards[] and children vector point to the same Card instances
            AddChild(card);
            
            index++;
        }
    }
    
    count = DECK_SIZE;
    TraceLog(LOG_INFO, "Deck: Created %d cards and added as children", count);
}

Deck::~Deck() {
    Cleanup();
}

void Deck::Update(float deltaTime) {
    (void)deltaTime;
    
    // Update card positions to stack them properly
    // Only the first 'count' cards in the array are active (still in deck)
    // The rest have been drawn and should be inactive
    float cardThickness = 0.02f;
    
    // First, make all cards inactive
    for (int i = 0; i < DECK_SIZE; i++) {
        if (cards[i] != nullptr) {
            cards[i]->isActive = false;
        }
    }
    
    // Then activate and position only the cards still in the deck
    for (int i = 0; i < count; i++) {
        if (cards[i] != nullptr) {
            cards[i]->isActive = true;
            cards[i]->position = {
                position.x,
                position.y + i * cardThickness,
                position.z
            };
            // Cards should lay flat face-down on the table
            // Rotate 90 degrees on X to lay flat, then 180 on Z to flip face-down
            cards[i]->rotation = {90.0f, 0.0f, 180.0f};
        }
    }
}

void Deck::Draw(Camera3D camera) {
    (void)camera;
    // Cards are rendered automatically by DrawWithChildren()
    // No need to manually draw them here
}

const char* Deck::GetType() const {
    return "deck";
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

Card* Deck::DrawCard() {
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
