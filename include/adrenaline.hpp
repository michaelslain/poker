#ifndef ADRENALINE_HPP
#define ADRENALINE_HPP

#include "substance.hpp"

class Adrenaline : public Substance {
public:
    Adrenaline(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Adrenaline();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Adrenaline"; }
};

#endif
