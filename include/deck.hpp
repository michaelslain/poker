#ifndef DECK_HPP
#define DECK_HPP

#include "object.hpp"
#include "card.hpp"
#include <array>

#define DECK_SIZE 52

class Deck : public Object {
private:
    std::array<Card*, DECK_SIZE> cards;
    int count;

public:
    Deck(Vector3 pos = {0, 0, 0});
    ~Deck();
    
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    const char* GetType() const override;
    
    void Shuffle();
    Card* DrawCard();  // Renamed from Draw to avoid conflict with Object::Draw
    Card* Peek();
    void Reset();
    void Cleanup();
    
    // Accessors
    int GetCount() const { return count; }
};

#endif
