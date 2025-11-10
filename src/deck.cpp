#include "deck.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

Deck::Deck(Vector3 pos) : Object(pos) {
    
    // Generate all 52 cards
    for (int suit = SUIT_HEARTS; suit <= SUIT_SPADES; suit++) {
        for (int rank = RANK_ACE; rank <= RANK_KING; rank++) {
            // Create card at origin with no physics (cards are part of the deck, not individual objects)
            Card* card = new Card(static_cast<Suit>(suit), static_cast<Rank>(rank), {0, 0, 0}, nullptr);
            
            // Add to allCards for cleanup tracking
            allCards.push_back(card);
            
            // Push onto stack
            cards.push_back(card);
        }
    }
    
}

Deck::~Deck() {
    Cleanup();
}

void Deck::Update(float deltaTime) {
    (void)deltaTime;
    
    // Update card positions to stack them properly
    // Only cards in the stack are active and rendered
    float cardThickness = 0.02f;
    
    // Position and activate only the cards still in the stack
    for (size_t i = 0; i < cards.size(); i++) {
        Card* card = cards[i];
        if (card != nullptr) {
            card->position = {
                position.x,
                position.y + i * cardThickness,
                position.z
            };
            // Cards should lay flat face-down on the table
            // Rotate 90 degrees on X to lay flat, then 180 on Z to flip face-down
            card->rotation = {90.0f, 0.0f, 180.0f};
        }
    }
}

void Deck::Draw(Camera3D camera) {
    // Draw all cards in the stack manually (they're not in DOM)
    for (Card* card : cards) {
        if (card) {
            card->Draw(camera);
        }
    }
}

std::string Deck::GetType() const {
    return "deck";
}

void Deck::Shuffle() {
    if (cards.size() <= 1) return;
    
    // Use std::shuffle with random device for proper random shuffling
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(cards.begin(), cards.end(), gen);
}

Card* Deck::DrawCard() {
    if (cards.empty()) {
        return nullptr;
    }
    
    // Pop from the top of the stack (back of vector)
    Card* drawnCard = cards.back();
    cards.pop_back();
    
    return drawnCard;
}

Card* Deck::Peek() {
    if (cards.empty()) {
        return nullptr;
    }
    
    return cards.back();
}

void Deck::Reset() {
    // Clear the stack
    cards.clear();
    
    // Push all cards back onto the stack
    for (Card* card : allCards) {
        if (card != nullptr) {
            cards.push_back(card);
        }
    }
    
}

void Deck::Cleanup() {
    // Clear the stack
    cards.clear();
    
    // Delete all cards
    for (Card* card : allCards) {
        if (card) {
            delete card;
        }
    }
    allCards.clear();
}
