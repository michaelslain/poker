#ifndef PISTOL_HPP
#define PISTOL_HPP

#include "weapons/weapon.hpp"

class Pistol : public Weapon {
public:
    Pistol(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Pistol();

    // Override virtual functions
    void Draw(Camera3D camera) override;
    void DrawIcon(Rectangle destRect) override;
    void DrawHeld(Camera3D camera) override;
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override;
};

#endif
