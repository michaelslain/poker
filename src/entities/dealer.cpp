#include "entities/dealer.hpp"

Dealer::Dealer(Vector3 pos, const std::string& name)
    : Person(pos, name) {
    // Dealers use default Person rendering (pitch black, usesLighting = false)
}

Dealer::~Dealer() {
    // Cleanup if needed
}

void Dealer::Update(float deltaTime) {
    // Call parent update
    Person::Update(deltaTime);
    
    // Dealer-specific update logic can go here
}

std::string Dealer::GetType() const {
    return Person::GetType() + "_dealer";
}
