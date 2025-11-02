#include "bullet.hpp"
#include "raymath.h"

Bullet::Bullet(Vector3 pos, Vector3 direction, float bulletSpeed)
    : Object(pos), lifetime(0.0f), maxLifetime(5.0f), speed(bulletSpeed)
{
    // Normalize direction and multiply by speed to get velocity
    velocity = Vector3Scale(Vector3Normalize(direction), speed);
    isDynamicallyAllocated = true;  // Bullets are always dynamically allocated
}

void Bullet::Update(float deltaTime) {
    if (!isActive) return;

    // Update lifetime
    lifetime += deltaTime;

    // Check if bullet has expired
    if (lifetime >= maxLifetime) {
        DeleteSelf();
        return;
    }

    // Move bullet based on velocity
    position = Vector3Add(position, Vector3Scale(velocity, deltaTime));
}

void Bullet::Draw(Camera3D camera) {
    (void)camera;
    if (!isActive) return;

    // Draw bullet as small gray capsule
    float radius = 0.05f;
    float height = 0.15f;

    // Calculate rotation to align with velocity direction
    Vector3 forward = Vector3Normalize(velocity);
    Vector3 up = {0, 1, 0};

    // If velocity is nearly vertical, use different up vector
    if (fabs(forward.y) > 0.99f) {
        up = {1, 0, 0};
    }

    Vector3 right = Vector3Normalize(Vector3CrossProduct(up, forward));
    up = Vector3CrossProduct(forward, right);

    // Draw capsule (cylinder with spheres on ends)
    DrawCapsule(position, Vector3Add(position, Vector3Scale(forward, height)), radius, 8, 8, DARKGRAY);
    DrawCapsuleWires(position, Vector3Add(position, Vector3Scale(forward, height)), radius, 8, 8, GRAY);
}

const char* Bullet::GetType() const {
    return "bullet";
}
