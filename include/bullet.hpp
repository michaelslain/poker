#ifndef BULLET_HPP
#define BULLET_HPP

#include "object.hpp"
#include "raylib.h"

class Bullet : public Object {
public:
    Vector3 velocity;
    float lifetime;
    float maxLifetime;
    float speed;

    Bullet(Vector3 pos, Vector3 direction, float bulletSpeed = 50.0f);
    virtual ~Bullet() = default;

    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    const char* GetType() const override;
    
    bool IsExpired() const { return lifetime >= maxLifetime; }
};

#endif
