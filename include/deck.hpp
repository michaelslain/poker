#ifndef DECK_HPP
#define DECK_HPP

#include "card.hpp"
#include <array>

#define DECK_SIZE 52

class Deck {
private:
    std::array<Card*, DECK_SIZE> cards;
    int count;

public:
    Deck();
    ~Deck();
    
    void Shuffle();
    Card* Draw();
    Card* Peek();
    void Reset();
    void Cleanup();
    
    // Accessors
    int GetCount() const { return count; }
};

#endif
