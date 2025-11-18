#ifndef DEALER_HPP
#define DEALER_HPP

#include "raylib.h"
#include "entities/person.hpp"
#include "core/physics.hpp"

class Dealer : public Person {
public:
    Dealer(Vector3 pos, const std::string& name = "Dealer");
    virtual ~Dealer();
    
    // Override Update to add dealer-specific behavior if needed
    void Update(float deltaTime) override;
    
    // Override GetType for identification
    std::string GetType() const override;
};

#endif
