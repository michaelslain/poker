#ifndef WEED_HPP
#define WEED_HPP

#include "substance.hpp"

class Weed : public Substance {
public:
    Weed(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Weed();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Weed"; }
};

#endif
