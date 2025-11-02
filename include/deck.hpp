#ifndef DECK_HPP
#define DECK_HPP

#include "object.hpp"
#include "card.hpp"
#include <vector>

#define DECK_SIZE 52

class Deck : public Object {
private:
    std::vector<Card*> cards;  // Stack of cards (back = top of deck)
    std::vector<Card*> allCards;  // All 52 cards for cleanup

public:
    Deck(Vector3 pos = {0, 0, 0});
    ~Deck();
    
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    const char* GetType() const override;
    
    void Shuffle();
    Card* DrawCard();  // Pop from top of stack
    Card* Peek();      // Look at top without removing
    void Reset();      // Push all cards back onto stack
    void Cleanup();
    
    // Accessors
    int GetCount() const { return cards.size(); }
    bool IsEmpty() const { return cards.empty(); }
};

#endif
