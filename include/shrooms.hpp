#ifndef SHROOMS_HPP
#define SHROOMS_HPP

#include "substance.hpp"

class Shrooms : public Substance {
public:
    Shrooms(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Shrooms();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Shrooms"; }
};

#endif
