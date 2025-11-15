#ifndef VODKA_HPP
#define VODKA_HPP

#include "substances/substance.hpp"

class Vodka : public Substance {
public:
    Vodka(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Vodka();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Vodka"; }
};

#endif
