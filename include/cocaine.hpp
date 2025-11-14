#ifndef COCAINE_HPP
#define COCAINE_HPP

#include "substance.hpp"

class Cocaine : public Substance {
public:
    Cocaine(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Cocaine();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Cocaine"; }
};

#endif
