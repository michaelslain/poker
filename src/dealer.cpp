#include "dealer.hpp"

Dealer::Dealer(Vector3 pos, const std::string& name)
    : Person(pos, name)
{
    // Dealer-specific initialization can go here
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
