#ifndef FENT_HPP
#define FENT_HPP

#include "substances/substance.hpp"

class Fent : public Substance {
public:
    Fent(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Fent();

    // Override virtual functions
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
    
    // Substance-specific methods
    void Consume() override;
    const char* GetName() const override { return "Fent"; }
};

#endif // FENT_HPP
