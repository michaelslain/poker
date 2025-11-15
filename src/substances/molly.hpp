#ifndef MOLLY_HPP
#define MOLLY_HPP

#include "substances/substance.hpp"

class Molly : public Substance {
public:
    Molly(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Molly();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Molly"; }
};

#endif
