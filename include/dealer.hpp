#ifndef DEALER_HPP
#define DEALER_HPP

#include "person.hpp"

class PhysicsWorld;

class Dealer : public Person {
public:
    Dealer(Vector3 pos, const std::string& name = "Dealer");
    ~Dealer();
    
    void Update(float deltaTime) override;
    const char* GetType() const override;
};

#endif
