#ifndef PISTOL_HPP
#define PISTOL_HPP

#include "item.hpp"
#include "rigidbody.hpp"
#include "physics.hpp"

class Pistol : public Item {
public:
    int ammo;
    int maxAmmo;
    RigidBody* rigidBody;

    Pistol(Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Pistol();

    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    void DrawIcon(Rectangle destRect) override;
    void DrawHeld(Camera3D camera);  // Draw when player is holding it
    std::string GetType() const override;
    
    bool CanShoot() const { return ammo > 0; }
    void Shoot();  // Decrements ammo
    int GetAmmo() const { return ammo; }
};

#endif
