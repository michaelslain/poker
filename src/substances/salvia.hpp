#ifndef SALVIA_HPP
#define SALVIA_HPP

#include "substances/substance.hpp"

class Salvia : public Substance {
public:
    Salvia(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Salvia();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Salvia"; }
};

#endif
